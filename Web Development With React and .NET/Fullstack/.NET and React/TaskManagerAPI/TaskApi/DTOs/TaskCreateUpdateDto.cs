namespace TaskApi.DTOs;

public class TaskCreateUpdateDto
{
    public string Title { get; set; } = string.Empty;
    public string? Description { get; set; }
    public string? Category { get; set; }
    public string Status { get; set; } = "Pending";
}