namespace EventReminder.API.Models;

public static class TaskStatuses
{
    public const string Pending = "Pending";
    public const string Completed = "Completed";
    public const string Notified = "Notified";

    private static readonly string[] Values = [Pending, Completed, Notified];

    public static string Normalize(string? status)
    {
        if (string.IsNullOrWhiteSpace(status))
        {
            return Pending;
        }

        var normalized = Values.FirstOrDefault(value =>
            string.Equals(value, status.Trim(), StringComparison.OrdinalIgnoreCase));

        if (normalized is null)
        {
            throw new ArgumentException($"Invalid status '{status}'. Valid values: {string.Join(", ", Values)}.");
        }

        return normalized;
    }
}
