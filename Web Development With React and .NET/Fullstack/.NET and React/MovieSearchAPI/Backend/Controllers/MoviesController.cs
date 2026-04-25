using Backend.Services;
using Microsoft.AspNetCore.Mvc;

namespace Backend.Controllers;

[ApiController]
[Route("api/[controller]")]
public class MoviesController(OmdbService omdbService) : ControllerBase
{
    [HttpGet("search")]
    public async Task<IActionResult> Search([FromQuery] string title, CancellationToken cancellationToken)
    {
        if (string.IsNullOrWhiteSpace(title))
        {
            return BadRequest(new { message = "Title query parameter is required." });
        }

        try
        {
            var results = await omdbService.SearchAsync(title.Trim(), cancellationToken);
            return Ok(results);
        }
        catch (InvalidOperationException exception)
        {
            return Problem(
                title: "OMDb configuration error",
                detail: exception.Message,
                statusCode: StatusCodes.Status500InternalServerError);
        }
    }
}
