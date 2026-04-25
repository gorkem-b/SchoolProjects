using System.Net.Http.Json;
using System.Text.Json.Serialization;
using Backend.DTOs;

namespace Backend.Services;

public class OmdbService(HttpClient httpClient, IConfiguration configuration)
{
    public async Task<IReadOnlyList<MovieSearchItemDto>> SearchAsync(string title, CancellationToken cancellationToken)
    {
        var apiKey = configuration["Omdb:ApiKey"];

        if (string.IsNullOrWhiteSpace(apiKey))
        {
            throw new InvalidOperationException("OMDb API key is missing. Configure Omdb:ApiKey with user-secrets or environment variables.");
        }

        var baseUrl = configuration["Omdb:BaseUrl"] ?? "https://www.omdbapi.com/";
        var requestUrl = $"{baseUrl}?apikey={Uri.EscapeDataString(apiKey)}&s={Uri.EscapeDataString(title)}";

        using var response = await httpClient.GetAsync(requestUrl, cancellationToken);

        if (!response.IsSuccessStatusCode)
        {
            if (response.StatusCode == System.Net.HttpStatusCode.Unauthorized)
            {
                throw new InvalidOperationException("OMDb request was rejected. Check whether your API key is valid.");
            }

            throw new InvalidOperationException($"OMDb request failed with status code {(int)response.StatusCode}.");
        }

        var payload = await response.Content.ReadFromJsonAsync<OmdbSearchResponse>(cancellationToken: cancellationToken);

        if (payload is null || !string.Equals(payload.Response, "True", StringComparison.OrdinalIgnoreCase) || payload.Search is null)
        {
            return [];
        }

        return payload.Search.Select(movie => new MovieSearchItemDto
        {
            ImdbId = movie.ImdbId ?? string.Empty,
            Title = movie.Title ?? string.Empty,
            Year = movie.Year ?? string.Empty,
            Type = movie.Type ?? string.Empty,
            PosterUrl = NormalizePoster(movie.Poster)
        }).ToList();
    }

    private static string? NormalizePoster(string? poster)
    {
        return string.Equals(poster, "N/A", StringComparison.OrdinalIgnoreCase) ? null : poster;
    }

    private sealed class OmdbSearchResponse
    {
        public List<OmdbMovieItem>? Search { get; set; }
        public string? Response { get; set; }
    }

    private sealed class OmdbMovieItem
    {
        public string? Title { get; set; }
        public string? Year { get; set; }
        public string? Type { get; set; }
        public string? Poster { get; set; }

        [JsonPropertyName("imdbID")]
        public string? ImdbId { get; set; }
    }
}
