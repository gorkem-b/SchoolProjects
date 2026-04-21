using System.Globalization;
using System.Text.Json;
using Microsoft.Extensions.Caching.Memory;
using WeatherInformationAPI.DTOs;

namespace WeatherInformationAPI.Services;

public class WeatherService(HttpClient httpClient, IMemoryCache memoryCache)
{
    private const string GeocodingBaseUrl = "https://geocoding-api.open-meteo.com/v1/search";
    private const string ForecastBaseUrl = "https://api.open-meteo.com/v1/forecast";
    private static readonly TimeSpan CacheDuration = TimeSpan.FromMinutes(10);
    private static readonly IReadOnlyDictionary<string, string> CountryNameOverrides =
        new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase)
        {
            ["TR"] = "Turkey"
        };

    public async Task<WeatherResponseDto> GetWeatherAsync(string city, CancellationToken cancellationToken = default)
    {
        var normalizedCity = city.Trim();
        var cacheKey = $"weather:{normalizedCity.ToLowerInvariant()}";

        if (memoryCache.TryGetValue(cacheKey, out WeatherResponseDto? cachedWeather) && cachedWeather is not null)
        {
            return cachedWeather;
        }

        var location = await GetLocationAsync(normalizedCity, cancellationToken);
        var forecastUrl = BuildForecastUrl(location.Latitude, location.Longitude);

        using var response = await httpClient.GetAsync(forecastUrl, cancellationToken);
        response.EnsureSuccessStatusCode();

        await using var responseStream = await response.Content.ReadAsStreamAsync(cancellationToken);
        using var document = await JsonDocument.ParseAsync(responseStream, cancellationToken: cancellationToken);

        if (!document.RootElement.TryGetProperty("current", out var current))
        {
            throw new InvalidOperationException("Weather provider returned an unexpected response.");
        }

        var weatherCode = current.GetProperty("weather_code").GetInt32();
        var (description, icon) = MapWeatherCode(weatherCode);

        var weather = new WeatherResponseDto
        {
            CityName = location.Name,
            Country = NormalizeCountryName(location.Country, location.CountryCode),
            Description = description,
            Icon = icon,
            Temperature = current.GetProperty("temperature_2m").GetDouble(),
            FeelsLike = current.GetProperty("apparent_temperature").GetDouble(),
            Humidity = current.GetProperty("relative_humidity_2m").GetInt32(),
            WindSpeed = current.GetProperty("wind_speed_10m").GetDouble()
        };

        memoryCache.Set(cacheKey, weather, CacheDuration);
        return weather;
    }

    private async Task<LocationResult> GetLocationAsync(string city, CancellationToken cancellationToken)
    {
        var geocodingUrl = $"{GeocodingBaseUrl}?name={Uri.EscapeDataString(city)}&count=1&language=en&format=json";
        using var response = await httpClient.GetAsync(geocodingUrl, cancellationToken);

        if (response.StatusCode == System.Net.HttpStatusCode.NotFound)
        {
            throw new KeyNotFoundException("City not found.");
        }

        response.EnsureSuccessStatusCode();

        await using var responseStream = await response.Content.ReadAsStreamAsync(cancellationToken);
        using var document = await JsonDocument.ParseAsync(responseStream, cancellationToken: cancellationToken);

        if (!document.RootElement.TryGetProperty("results", out var results) || results.GetArrayLength() == 0)
        {
            throw new KeyNotFoundException("City not found.");
        }

        var location = results[0];

        return new LocationResult(
            location.GetProperty("name").GetString() ?? city,
            location.GetProperty("country").GetString() ?? string.Empty,
            location.GetProperty("country_code").GetString() ?? string.Empty,
            location.GetProperty("latitude").GetDouble(),
            location.GetProperty("longitude").GetDouble());
    }

    private static string BuildForecastUrl(double latitude, double longitude)
    {
        var latitudeValue = latitude.ToString(CultureInfo.InvariantCulture);
        var longitudeValue = longitude.ToString(CultureInfo.InvariantCulture);

        return $"{ForecastBaseUrl}?latitude={latitudeValue}&longitude={longitudeValue}&current=temperature_2m,apparent_temperature,relative_humidity_2m,weather_code,wind_speed_10m&timezone=auto";
    }

    private static (string Description, string Icon) MapWeatherCode(int weatherCode) => weatherCode switch
    {
        0 => ("Clear sky", "☀️"),
        1 => ("Mainly clear", "🌤️"),
        2 => ("Partly cloudy", "⛅"),
        3 => ("Overcast", "☁️"),
        45 or 48 => ("Fog", "🌫️"),
        51 or 53 or 55 => ("Drizzle", "🌦️"),
        56 or 57 => ("Freezing drizzle", "🧊"),
        61 or 63 or 65 => ("Rain", "🌧️"),
        66 or 67 => ("Freezing rain", "🌧️"),
        71 or 73 or 75 => ("Snow", "❄️"),
        77 => ("Snow grains", "🌨️"),
        80 or 81 or 82 => ("Rain showers", "🌦️"),
        85 or 86 => ("Snow showers", "🌨️"),
        95 => ("Thunderstorm", "⛈️"),
        96 or 99 => ("Thunderstorm with hail", "⛈️"),
        _ => ("Unknown conditions", "🌍")
    };

    private static string NormalizeCountryName(string countryName, string countryCode)
    {
        if (CountryNameOverrides.TryGetValue(countryCode, out var overriddenName))
        {
            return overriddenName;
        }

        return countryName;
    }

    private sealed record LocationResult(string Name, string Country, string CountryCode, double Latitude, double Longitude);
}