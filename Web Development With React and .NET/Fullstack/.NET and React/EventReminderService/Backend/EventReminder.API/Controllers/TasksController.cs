using EventReminder.API.Models;
using EventReminder.API.Services;
using Microsoft.AspNetCore.Mvc;

namespace EventReminder.API.Controllers;

[ApiController]
[Route("api/[controller]")]
public class TasksController(ITaskService taskService) : ControllerBase
{
    [HttpGet]
    public async Task<ActionResult<IEnumerable<TaskDto>>> GetTasks(
        [FromQuery] string? category,
        [FromQuery] string? status,
        [FromQuery] bool? isNotified,
        CancellationToken cancellationToken)
    {
        try
        {
            var tasks = await taskService.GetTasksAsync(category, status, isNotified, cancellationToken);
            return Ok(tasks);
        }
        catch (ArgumentException ex)
        {
            return BadRequest(new { message = ex.Message });
        }
    }

    [HttpGet("{id:int}")]
    public async Task<ActionResult<TaskDto>> GetTask(int id, CancellationToken cancellationToken)
    {
        var task = await taskService.GetTaskByIdAsync(id, cancellationToken);

        if (task is null)
        {
            return NotFound(new { message = $"Task with id {id} was not found." });
        }

        return Ok(task);
    }

    [HttpPost]
    public async Task<ActionResult<TaskDto>> CreateTask(CreateTaskDto dto, CancellationToken cancellationToken)
    {
        try
        {
            var createdTask = await taskService.CreateTaskAsync(dto, cancellationToken);
            return CreatedAtAction(nameof(GetTask), new { id = createdTask.Id }, createdTask);
        }
        catch (ArgumentException ex)
        {
            return BadRequest(new { message = ex.Message });
        }
    }

    [HttpPut("{id:int}")]
    public async Task<ActionResult<TaskDto>> UpdateTask(int id, UpdateTaskDto dto, CancellationToken cancellationToken)
    {
        try
        {
            var updatedTask = await taskService.UpdateTaskAsync(id, dto, cancellationToken);
            return Ok(updatedTask);
        }
        catch (KeyNotFoundException ex)
        {
            return NotFound(new { message = ex.Message });
        }
        catch (ArgumentException ex)
        {
            return BadRequest(new { message = ex.Message });
        }
    }

    [HttpDelete("{id:int}")]
    public async Task<IActionResult> DeleteTask(int id, CancellationToken cancellationToken)
    {
        var deleted = await taskService.DeleteTaskAsync(id, cancellationToken);

        if (!deleted)
        {
            return NotFound(new { message = $"Task with id {id} was not found." });
        }

        return NoContent();
    }
}
