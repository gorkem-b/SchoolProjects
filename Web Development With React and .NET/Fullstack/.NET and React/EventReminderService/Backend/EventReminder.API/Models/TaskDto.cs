using System.ComponentModel.DataAnnotations;

namespace EventReminder.API.Models;

public class TaskDto
{
    public int Id { get; set; }
    public string Title { get; set; } = string.Empty;
    public string? Description { get; set; }
    public DateTime DueDate { get; set; }
    public string Category { get; set; } = string.Empty;
    public string Status { get; set; } = string.Empty;
    public bool IsNotified { get; set; }
    public DateTime CreatedAt { get; set; }
    public DateTime? UpdatedAt { get; set; }

    public static TaskDto FromEntity(TaskItem task) => new()
    {
        Id = task.Id,
        Title = task.Title,
        Description = task.Description,
        DueDate = task.DueDate,
        Category = task.Category,
        Status = task.Status,
        IsNotified = task.IsNotified,
        CreatedAt = task.CreatedAt,
        UpdatedAt = task.UpdatedAt
    };
}

public class CreateTaskDto
{
    [Required]
    [MaxLength(120)]
    public string Title { get; set; } = string.Empty;

    [MaxLength(1000)]
    public string? Description { get; set; }

    public DateTime DueDate { get; set; }

    [MaxLength(80)]
    public string? Category { get; set; }
}

public class UpdateTaskDto
{
    [Required]
    [MaxLength(120)]
    public string Title { get; set; } = string.Empty;

    [MaxLength(1000)]
    public string? Description { get; set; }

    public DateTime DueDate { get; set; }

    [MaxLength(80)]
    public string? Category { get; set; }

    public string Status { get; set; } = TaskStatuses.Pending;
    public bool IsNotified { get; set; }
}
