using Microsoft.EntityFrameworkCore;
using WeatherInformationAPI.Models;

namespace WeatherInformationAPI.Data;

public class AppDbContext(DbContextOptions<AppDbContext> options) : DbContext(options)
{
    public DbSet<WeatherTask> WeatherTasks => Set<WeatherTask>();

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);

        var task = modelBuilder.Entity<WeatherTask>();

        task.Property(item => item.Title)
            .HasMaxLength(100)
            .IsRequired();

        task.Property(item => item.Description)
            .HasMaxLength(500)
            .IsRequired();

        task.Property(item => item.Category)
            .HasMaxLength(50)
            .IsRequired();

        task.Property(item => item.CityName)
            .HasMaxLength(100)
            .IsRequired();

        task.Property(item => item.Status)
            .HasConversion<string>()
            .HasMaxLength(20)
            .IsRequired();

        task.Property(item => item.CreatedAt)
            .IsRequired();
    }
}
