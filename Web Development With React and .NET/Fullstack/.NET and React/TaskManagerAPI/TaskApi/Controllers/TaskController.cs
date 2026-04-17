using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using System.Security.Claims;
using TaskApi.DTOs;
using TaskApi.Services;

namespace TaskApi.Controllers;

[ApiController]
[Route("api/tasks")]
[Authorize]
public class TaskController : ControllerBase
{
    private readonly TaskService _taskService;

    public TaskController(TaskService taskService)
    {
        _taskService = taskService;
    }

    [HttpGet]
    public async Task<IActionResult> GetTasks()
    {
        var userId = GetUserId();
        if (userId is null)
        {
            return Unauthorized(new { message = "A valid user identity could not be found." });
        }

        var tasks = await _taskService.GetUserTasksAsync(userId.Value);
        return Ok(tasks);
    }

    [HttpPost]
    public async Task<IActionResult> CreateTask([FromBody] TaskCreateUpdateDto dto)
    {
        if (string.IsNullOrWhiteSpace(dto.Title))
        {
            return BadRequest(new { message = "Task title is required." });
        }

        var userId = GetUserId();
        if (userId is null)
        {
            return Unauthorized(new { message = "A valid user identity could not be found." });
        }

        var createdTask = await _taskService.CreateTaskAsync(userId.Value, dto);
        return CreatedAtAction(nameof(GetTasks), new { id = createdTask.Id }, createdTask);
    }

    [HttpPut("{id:int}")]
    public async Task<IActionResult> UpdateTask(int id, [FromBody] TaskCreateUpdateDto dto)
    {
        if (string.IsNullOrWhiteSpace(dto.Title))
        {
            return BadRequest(new { message = "Task title is required." });
        }

        var userId = GetUserId();
        if (userId is null)
        {
            return Unauthorized(new { message = "A valid user identity could not be found." });
        }

        var updated = await _taskService.UpdateTaskAsync(id, userId.Value, dto);
        if (!updated)
        {
            return NotFound(new { message = "Task not found." });
        }

        return NoContent();
    }

    [HttpDelete("{id:int}")]
    public async Task<IActionResult> DeleteTask(int id)
    {
        var userId = GetUserId();
        if (userId is null)
        {
            return Unauthorized(new { message = "A valid user identity could not be found." });
        }

        var deleted = await _taskService.DeleteTaskAsync(id, userId.Value);
        if (!deleted)
        {
            return NotFound(new { message = "Task not found." });
        }

        return NoContent();
    }

    private int? GetUserId()
    {
        var userIdValue = User.FindFirstValue(ClaimTypes.NameIdentifier);
        if (!int.TryParse(userIdValue, out var userId))
        {
            return null;
        }

        return userId;
    }
}