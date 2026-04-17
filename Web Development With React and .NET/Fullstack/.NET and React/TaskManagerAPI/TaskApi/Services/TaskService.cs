using Microsoft.EntityFrameworkCore;
using TaskApi.Data;
using TaskApi.DTOs;
using TaskApi.Models;

namespace TaskApi.Services;

public class TaskService
{
    private readonly AppDbContext _context;

    public TaskService(AppDbContext context)
    {
        _context = context;
    }

    public async Task<IEnumerable<TaskResponseDto>> GetUserTasksAsync(int userId)
    {
        var tasks = await _context.TaskItems
            .Where(task => task.UserId == userId)
            .OrderByDescending(task => task.Id)
            .Select(task => ToResponseDto(task))
            .ToListAsync();

        return tasks;
    }

    public async Task<TaskResponseDto> CreateTaskAsync(int userId, TaskCreateUpdateDto dto)
    {
        var taskItem = new TaskItem
        {
            Title = dto.Title.Trim(),
            Description = NormalizeNullable(dto.Description),
            Category = NormalizeNullable(dto.Category),
            Status = NormalizeStatus(dto.Status),
            UserId = userId
        };

        _context.TaskItems.Add(taskItem);
        await _context.SaveChangesAsync();

        return ToResponseDto(taskItem);
    }

    public async Task<bool> UpdateTaskAsync(int taskId, int userId, TaskCreateUpdateDto dto)
    {
        var taskItem = await _context.TaskItems
            .SingleOrDefaultAsync(task => task.Id == taskId && task.UserId == userId);

        if (taskItem is null)
        {
            return false;
        }

        taskItem.Title = dto.Title.Trim();
        taskItem.Description = NormalizeNullable(dto.Description);
        taskItem.Category = NormalizeNullable(dto.Category);
        taskItem.Status = NormalizeStatus(dto.Status);

        await _context.SaveChangesAsync();

        return true;
    }

    public async Task<bool> DeleteTaskAsync(int taskId, int userId)
    {
        var taskItem = await _context.TaskItems
            .SingleOrDefaultAsync(task => task.Id == taskId && task.UserId == userId);

        if (taskItem is null)
        {
            return false;
        }

        _context.TaskItems.Remove(taskItem);
        await _context.SaveChangesAsync();

        return true;
    }

    private static TaskResponseDto ToResponseDto(TaskItem taskItem)
    {
        return new TaskResponseDto
        {
            Id = taskItem.Id,
            Title = taskItem.Title,
            Description = taskItem.Description,
            Category = taskItem.Category,
            Status = taskItem.Status
        };
    }

    private static string? NormalizeNullable(string? value)
    {
        return string.IsNullOrWhiteSpace(value) ? null : value.Trim();
    }

    private static string NormalizeStatus(string? value)
    {
        return string.IsNullOrWhiteSpace(value) ? "Pending" : value.Trim();
    }
}