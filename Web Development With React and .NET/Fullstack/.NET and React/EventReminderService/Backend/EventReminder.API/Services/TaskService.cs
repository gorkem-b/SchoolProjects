using EventReminder.API.Data;
using EventReminder.API.Models;
using Microsoft.EntityFrameworkCore;

namespace EventReminder.API.Services;

public class TaskService(AppDbContext dbContext) : ITaskService
{
    public async Task<IEnumerable<TaskDto>> GetTasksAsync(
        string? category,
        string? status,
        bool? isNotified,
        CancellationToken cancellationToken = default)
    {
        var query = dbContext.TaskItems.AsNoTracking().AsQueryable();

        if (!string.IsNullOrWhiteSpace(category))
        {
            query = query.Where(task => task.Category == category.Trim());
        }

        if (!string.IsNullOrWhiteSpace(status))
        {
            var normalizedStatus = TaskStatuses.Normalize(status);
            query = query.Where(task => task.Status == normalizedStatus);
        }

        if (isNotified.HasValue)
        {
            query = query.Where(task => task.IsNotified == isNotified.Value);
        }

        var tasks = await query
            .OrderBy(task => task.DueDate)
            .ToListAsync(cancellationToken);

        return tasks.Select(TaskDto.FromEntity);
    }

    public async Task<TaskDto?> GetTaskByIdAsync(int id, CancellationToken cancellationToken = default)
    {
        var task = await dbContext.TaskItems
            .AsNoTracking()
            .FirstOrDefaultAsync(item => item.Id == id, cancellationToken);

        return task is null ? null : TaskDto.FromEntity(task);
    }

    public async Task<TaskDto> CreateTaskAsync(CreateTaskDto dto, CancellationToken cancellationToken = default)
    {
        ValidateTitle(dto.Title);
        ValidateDueDate(dto.DueDate);

        var now = DateTime.UtcNow;
        var task = new TaskItem
        {
            Title = dto.Title.Trim(),
            Description = NormalizeOptionalText(dto.Description),
            DueDate = NormalizeDate(dto.DueDate),
            Category = NormalizeCategory(dto.Category),
            Status = TaskStatuses.Pending,
            IsNotified = false,
            CreatedAt = now
        };

        dbContext.TaskItems.Add(task);
        await dbContext.SaveChangesAsync(cancellationToken);

        return TaskDto.FromEntity(task);
    }

    public async Task<TaskDto> UpdateTaskAsync(int id, UpdateTaskDto dto, CancellationToken cancellationToken = default)
    {
        ValidateTitle(dto.Title);
        ValidateDueDate(dto.DueDate);

        var task = await dbContext.TaskItems.FindAsync([id], cancellationToken)
            ?? throw new KeyNotFoundException($"Task with id {id} was not found.");

        var status = TaskStatuses.Normalize(dto.Status);

        task.Title = dto.Title.Trim();
        task.Description = NormalizeOptionalText(dto.Description);
        task.DueDate = NormalizeDate(dto.DueDate);
        task.Category = NormalizeCategory(dto.Category);
        task.Status = status;
        task.IsNotified = status == TaskStatuses.Notified || dto.IsNotified;
        task.UpdatedAt = DateTime.UtcNow;

        await dbContext.SaveChangesAsync(cancellationToken);

        return TaskDto.FromEntity(task);
    }

    public async Task<bool> DeleteTaskAsync(int id, CancellationToken cancellationToken = default)
    {
        var task = await dbContext.TaskItems.FindAsync([id], cancellationToken);

        if (task is null)
        {
            return false;
        }

        dbContext.TaskItems.Remove(task);
        await dbContext.SaveChangesAsync(cancellationToken);
        return true;
    }

    private static void ValidateTitle(string? title)
    {
        if (string.IsNullOrWhiteSpace(title))
        {
            throw new ArgumentException("Title is required.");
        }
    }

    private static void ValidateDueDate(DateTime dueDate)
    {
        if (dueDate == default)
        {
            throw new ArgumentException("Due date is required.");
        }
    }

    private static string NormalizeCategory(string? category) =>
        string.IsNullOrWhiteSpace(category) ? "General" : category.Trim();

    private static string? NormalizeOptionalText(string? value) =>
        string.IsNullOrWhiteSpace(value) ? null : value.Trim();

    private static DateTime NormalizeDate(DateTime value) =>
        value.Kind == DateTimeKind.Unspecified
            ? DateTime.SpecifyKind(value, DateTimeKind.Utc)
            : value.ToUniversalTime();
}
