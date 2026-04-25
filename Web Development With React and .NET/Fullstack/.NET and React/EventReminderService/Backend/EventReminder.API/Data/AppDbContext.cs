using EventReminder.API.Models;
using Microsoft.EntityFrameworkCore;

namespace EventReminder.API.Data;

public class AppDbContext(DbContextOptions<AppDbContext> options) : DbContext(options)
{
    public DbSet<TaskItem> TaskItems => Set<TaskItem>();

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        var task = modelBuilder.Entity<TaskItem>();

        task.HasKey(item => item.Id);
        task.Property(item => item.Title).IsRequired().HasMaxLength(120);
        task.Property(item => item.Description).HasMaxLength(1000);
        task.Property(item => item.Category).IsRequired().HasMaxLength(80);
        task.Property(item => item.Status)
            .IsRequired()
            .HasMaxLength(30)
            .HasDefaultValue(TaskStatuses.Pending);
        task.Property(item => item.IsNotified).HasDefaultValue(false);
        task.Property(item => item.CreatedAt).HasDefaultValueSql("SYSUTCDATETIME()");

        task.HasIndex(item => item.DueDate);
        task.HasIndex(item => new { item.Status, item.IsNotified });
    }
}
