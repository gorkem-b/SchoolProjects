using Microsoft.AspNetCore.Mvc;
using WeatherInformationAPI.Services;

namespace WeatherInformationAPI.Controllers;

[ApiController]
[Route("api/[controller]")]
public class WeatherController(WeatherService weatherService) : ControllerBase
{
    [HttpGet("{city}")]
    public async Task<IActionResult> GetWeather(string city, CancellationToken cancellationToken)
    {
        if (string.IsNullOrWhiteSpace(city))
        {
            return BadRequest(new { message = "City name is required." });
        }

        try
        {
            var weather = await weatherService.GetWeatherAsync(city, cancellationToken);
            return Ok(weather);
        }
        catch (KeyNotFoundException)
        {
            return NotFound(new { message = "City not found." });
        }
        catch (InvalidOperationException exception)
        {
            return StatusCode(StatusCodes.Status500InternalServerError, new { message = exception.Message });
        }
        catch (HttpRequestException)
        {
            return StatusCode(StatusCodes.Status502BadGateway, new { message = "Weather provider is unavailable." });
        }
    }
}
