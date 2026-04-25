using EventReminder.API.Models;

namespace EventReminder.API.Services;

public interface ITaskService
{
    Task<IEnumerable<TaskDto>> GetTasksAsync(string? category, string? status, bool? isNotified, CancellationToken cancellationToken = default);
    Task<TaskDto?> GetTaskByIdAsync(int id, CancellationToken cancellationToken = default);
    Task<TaskDto> CreateTaskAsync(CreateTaskDto dto, CancellationToken cancellationToken = default);
    Task<TaskDto> UpdateTaskAsync(int id, UpdateTaskDto dto, CancellationToken cancellationToken = default);
    Task<bool> DeleteTaskAsync(int id, CancellationToken cancellationToken = default);
}
