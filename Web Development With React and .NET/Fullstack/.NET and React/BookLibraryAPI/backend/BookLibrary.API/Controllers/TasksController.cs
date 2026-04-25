using BookLibrary.Business.Interfaces;
using BookLibrary.Core.DTOs.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace BookLibrary.API.Controllers;

[ApiController]
[Route("api/[controller]")]
public class TasksController(ITaskService taskService) : ControllerBase
{
    private readonly ITaskService _taskService = taskService;

    [HttpGet]
    public async Task<ActionResult<IReadOnlyCollection<BookTaskDto>>> GetTasks()
    {
        var tasks = await _taskService.GetAllAsync();
        return Ok(tasks);
    }

    [HttpGet("{id:int}")]
    public async Task<ActionResult<BookTaskDto>> GetTask(int id)
    {
        var task = await _taskService.GetByIdAsync(id);
        if (task is null)
        {
            return NotFound();
        }

        return Ok(task);
    }

    [HttpPost]
    public async Task<ActionResult<BookTaskDto>> AddTask([FromBody] CreateBookTaskDto createBookTaskDto)
    {
        var createdTask = await _taskService.CreateAsync(createBookTaskDto);
        if (createdTask is null)
        {
            return BadRequest("Selected book was not found.");
        }

        return CreatedAtAction(nameof(GetTask), new { id = createdTask.Id }, createdTask);
    }

    [HttpPut("{id:int}")]
    public async Task<IActionResult> UpdateTask(int id, [FromBody] UpdateBookTaskDto updateBookTaskDto)
    {
        var updated = await _taskService.UpdateAsync(id, updateBookTaskDto);
        if (!updated)
        {
            return NotFound();
        }

        return NoContent();
    }

    [HttpDelete("{id:int}")]
    public async Task<IActionResult> DeleteTask(int id)
    {
        var deleted = await _taskService.DeleteAsync(id);
        if (!deleted)
        {
            return NotFound();
        }

        return NoContent();
    }
}