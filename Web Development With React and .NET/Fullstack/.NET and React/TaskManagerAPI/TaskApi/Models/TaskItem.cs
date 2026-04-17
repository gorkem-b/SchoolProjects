namespace TaskApi.Models;

public class TaskItem
{
    public int Id { get; set; }
    public string Title { get; set; } = string.Empty;
    public string? Description { get; set; }
    public string? Category { get; set; }
    public string Status { get; set; } = "Pending";
    public int UserId { get; set; }
    public User? User { get; set; }
}