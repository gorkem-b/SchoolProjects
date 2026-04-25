using Backend.Data;
using Backend.DTOs;
using Backend.Models;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;

namespace Backend.Controllers;

[ApiController]
[Route("api/[controller]")]
public class TasksController(ApplicationDbContext dbContext) : ControllerBase
{
    [HttpGet]
    public async Task<ActionResult<IEnumerable<MovieTaskDto>>> GetAll([FromQuery] bool? isWatched, CancellationToken cancellationToken)
    {
        var query = dbContext.Tasks.AsNoTracking().AsQueryable();

        if (isWatched.HasValue)
        {
            query = query.Where(task => task.IsWatched == isWatched.Value);
        }

        var tasks = await query
            .OrderByDescending(task => task.Id)
            .Select(task => ToDto(task))
            .ToListAsync(cancellationToken);

        return Ok(tasks);
    }

    [HttpGet("{id:int}")]
    public async Task<ActionResult<MovieTaskDto>> GetById(int id, CancellationToken cancellationToken)
    {
        var task = await dbContext.Tasks.AsNoTracking().FirstOrDefaultAsync(item => item.Id == id, cancellationToken);

        if (task is null)
        {
            return NotFound();
        }

        return Ok(ToDto(task));
    }

    [HttpPost]
    public async Task<ActionResult<MovieTaskDto>> Create([FromBody] UpsertMovieTaskDto request, CancellationToken cancellationToken)
    {
        var task = new MovieTask
        {
            Title = request.Title.Trim(),
            Description = request.Description?.Trim(),
            Category = request.Category.Trim(),
            IsWatched = request.IsWatched,
            Year = request.Year?.Trim(),
            PosterUrl = request.PosterUrl?.Trim()
        };

        dbContext.Tasks.Add(task);
        await dbContext.SaveChangesAsync(cancellationToken);

        return CreatedAtAction(nameof(GetById), new { id = task.Id }, ToDto(task));
    }

    [HttpPut("{id:int}")]
    public async Task<ActionResult<MovieTaskDto>> Update(int id, [FromBody] UpsertMovieTaskDto request, CancellationToken cancellationToken)
    {
        var task = await dbContext.Tasks.FirstOrDefaultAsync(item => item.Id == id, cancellationToken);

        if (task is null)
        {
            return NotFound();
        }

        task.Title = request.Title.Trim();
        task.Description = request.Description?.Trim();
        task.Category = request.Category.Trim();
        task.IsWatched = request.IsWatched;
        task.Year = request.Year?.Trim();
        task.PosterUrl = request.PosterUrl?.Trim();

        await dbContext.SaveChangesAsync(cancellationToken);

        return Ok(ToDto(task));
    }

    [HttpDelete("{id:int}")]
    public async Task<IActionResult> Delete(int id, CancellationToken cancellationToken)
    {
        var task = await dbContext.Tasks.FirstOrDefaultAsync(item => item.Id == id, cancellationToken);

        if (task is null)
        {
            return NotFound();
        }

        dbContext.Tasks.Remove(task);
        await dbContext.SaveChangesAsync(cancellationToken);

        return NoContent();
    }

    private static MovieTaskDto ToDto(MovieTask task)
    {
        return new MovieTaskDto
        {
            Id = task.Id,
            Title = task.Title,
            Description = task.Description,
            Category = task.Category,
            IsWatched = task.IsWatched,
            Year = task.Year,
            PosterUrl = task.PosterUrl
        };
    }
}
