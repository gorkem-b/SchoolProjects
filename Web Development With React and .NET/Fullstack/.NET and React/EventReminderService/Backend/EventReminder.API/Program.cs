using EventReminder.API.Data;
using EventReminder.API.Services;
using EventReminder.API.Workers;
using Microsoft.EntityFrameworkCore;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddControllers();
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

builder.Services.AddCors(options =>
{
    options.AddPolicy("Frontend", policy =>
    {
        policy.WithOrigins(
                "http://localhost:3000",
                "http://localhost:5173",
                "http://127.0.0.1:3000",
                "http://127.0.0.1:5173")
            .AllowAnyHeader()
            .AllowAnyMethod();
    });
});

var connectionString = builder.Configuration.GetConnectionString("DefaultConnection")
    ?? throw new InvalidOperationException("Connection string 'DefaultConnection' was not found.");

builder.Services.AddDbContext<AppDbContext>(options =>
    options.UseSqlServer(connectionString));

builder.Services.AddScoped<ITaskService, TaskService>();
builder.Services.AddHostedService<ReminderWorker>();

var app = builder.Build();

await MigrateDatabaseAsync(app);

app.UseSwagger();
app.UseSwaggerUI();

app.UseCors("Frontend");
app.Use(async (context, next) =>
{
    try
    {
        await next();
    }
    catch (Exception ex)
    {
        var logger = context.RequestServices.GetRequiredService<ILoggerFactory>()
            .CreateLogger("UnhandledException");

        logger.LogError(ex, "Unhandled API exception.");

        if (context.Response.HasStarted)
        {
            throw;
        }

        context.Response.StatusCode = StatusCodes.Status500InternalServerError;
        context.Response.ContentType = "application/json";
        await context.Response.WriteAsJsonAsync(new
        {
            message = "API request failed. Check backend logs and database migrations."
        });
    }
});
app.UseAuthorization();

app.MapControllers();
app.MapGet("/health", () => Results.Ok(new { status = "healthy", service = "EventReminder.API" }));
app.MapGet("/", () => Results.Redirect("/swagger"));

app.Run();

static async Task MigrateDatabaseAsync(WebApplication app)
{
    using var scope = app.Services.CreateScope();
    var dbContext = scope.ServiceProvider.GetRequiredService<AppDbContext>();
    var logger = scope.ServiceProvider.GetRequiredService<ILoggerFactory>()
        .CreateLogger("DatabaseMigration");

    const int maxRetries = 30;

    for (var attempt = 1; attempt <= maxRetries; attempt++)
    {
        try
        {
            await dbContext.Database.MigrateAsync();
            await EnsureTaskItemsTableExistsAsync(dbContext, logger);
            return;
        }
        catch (Exception ex) when (attempt < maxRetries)
        {
            logger.LogWarning(ex, "Database migration failed. Retrying attempt {Attempt}/{MaxRetries}...", attempt, maxRetries);
            await Task.Delay(TimeSpan.FromSeconds(5));
        }
    }

    await dbContext.Database.MigrateAsync();
    await EnsureTaskItemsTableExistsAsync(dbContext, logger);
}

static async Task EnsureTaskItemsTableExistsAsync(AppDbContext dbContext, ILogger logger)
{
    var connection = dbContext.Database.GetDbConnection();
    await using var _ = connection;

    if (connection.State != System.Data.ConnectionState.Open)
    {
        await connection.OpenAsync();
    }

    await using var command = connection.CreateCommand();
    command.CommandText = "SELECT CASE WHEN OBJECT_ID(N'[dbo].[TaskItems]', N'U') IS NULL THEN 0 ELSE 1 END";

    var result = await command.ExecuteScalarAsync();
    var tableExists = Convert.ToInt32(result) == 1;

    if (!tableExists)
    {
        throw new InvalidOperationException("Database migration completed, but the TaskItems table was not found.");
    }

    logger.LogInformation("Database schema verified: TaskItems table exists.");
}
