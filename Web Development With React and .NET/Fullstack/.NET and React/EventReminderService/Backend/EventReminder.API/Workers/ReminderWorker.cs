using EventReminder.API.Data;
using EventReminder.API.Models;
using Microsoft.EntityFrameworkCore;

namespace EventReminder.API.Workers;

public class ReminderWorker(IServiceScopeFactory scopeFactory, ILogger<ReminderWorker> logger) : BackgroundService
{
    private static readonly TimeSpan Interval = TimeSpan.FromMinutes(1);

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        logger.LogInformation("Reminder worker started.");

        while (!stoppingToken.IsCancellationRequested)
        {
            try
            {
                await ProcessDueTasksAsync(stoppingToken);
            }
            catch (Exception ex)
            {
                logger.LogError(ex, "Reminder worker failed while processing due tasks.");
            }

            try
            {
                await Task.Delay(Interval, stoppingToken);
            }
            catch (OperationCanceledException)
            {
                break;
            }
        }
    }

    private async Task ProcessDueTasksAsync(CancellationToken cancellationToken)
    {
        using var scope = scopeFactory.CreateScope();
        var dbContext = scope.ServiceProvider.GetRequiredService<AppDbContext>();
        var now = DateTime.UtcNow;

        var dueTasks = await dbContext.TaskItems
            .Where(task => task.DueDate <= now
                && task.Status == TaskStatuses.Pending
                && !task.IsNotified)
            .ToListAsync(cancellationToken);

        if (dueTasks.Count == 0)
        {
            return;
        }

        foreach (var task in dueTasks)
        {
            logger.LogInformation(
                "Reminder triggered for task {TaskId}: {Title} ({DueDate:u})",
                task.Id,
                task.Title,
                task.DueDate);

            task.Status = TaskStatuses.Notified;
            task.IsNotified = true;
            task.UpdatedAt = now;
        }

        await dbContext.SaveChangesAsync(cancellationToken);
        logger.LogInformation("Reminder worker processed {Count} due task(s).", dueTasks.Count);
    }
}
