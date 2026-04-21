using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using WeatherInformationAPI.Data;
using WeatherInformationAPI.DTOs;
using WeatherInformationAPI.Models;

namespace WeatherInformationAPI.Controllers;

[ApiController]
[Route("api/[controller]")]
public class TasksController(AppDbContext dbContext) : ControllerBase
{
    [HttpGet]
    public async Task<ActionResult<IEnumerable<WeatherTask>>> GetTasks(CancellationToken cancellationToken)
    {
        var tasks = await dbContext.WeatherTasks
            .AsNoTracking()
            .OrderByDescending(task => task.CreatedAt)
            .ToListAsync(cancellationToken);

        return Ok(tasks);
    }

    [HttpGet("{id:int}")]
    public async Task<ActionResult<WeatherTask>> GetTaskById(int id, CancellationToken cancellationToken)
    {
        var task = await dbContext.WeatherTasks
            .AsNoTracking()
            .FirstOrDefaultAsync(item => item.Id == id, cancellationToken);

        if (task is null)
        {
            return NotFound(new { message = "Task not found." });
        }

        return Ok(task);
    }

    [HttpPost]
    public async Task<ActionResult<WeatherTask>> CreateTask(TaskCreateDto request, CancellationToken cancellationToken)
    {
        var task = new WeatherTask
        {
            Title = request.Title.Trim(),
            Description = request.Description?.Trim() ?? string.Empty,
            Category = request.Category.Trim(),
            CityName = request.CityName.Trim(),
            Status = WeatherTaskStatus.Pending,
            CreatedAt = DateTime.UtcNow
        };

        dbContext.WeatherTasks.Add(task);
        await dbContext.SaveChangesAsync(cancellationToken);

        return CreatedAtAction(nameof(GetTaskById), new { id = task.Id }, task);
    }

    [HttpPut("{id:int}")]
    public async Task<ActionResult<WeatherTask>> UpdateTask(int id, TaskUpdateDto request, CancellationToken cancellationToken)
    {
        var task = await dbContext.WeatherTasks.FirstOrDefaultAsync(item => item.Id == id, cancellationToken);

        if (task is null)
        {
            return NotFound(new { message = "Task not found." });
        }

        task.Title = request.Title.Trim();
        task.Description = request.Description?.Trim() ?? string.Empty;
        task.Category = request.Category.Trim();
        task.CityName = request.CityName.Trim();
        task.Status = request.Status;

        await dbContext.SaveChangesAsync(cancellationToken);

        return Ok(task);
    }

    [HttpDelete("{id:int}")]
    public async Task<IActionResult> DeleteTask(int id, CancellationToken cancellationToken)
    {
        var task = await dbContext.WeatherTasks.FirstOrDefaultAsync(item => item.Id == id, cancellationToken);

        if (task is null)
        {
            return NotFound(new { message = "Task not found." });
        }

        dbContext.WeatherTasks.Remove(task);
        await dbContext.SaveChangesAsync(cancellationToken);

        return NoContent();
    }
}
