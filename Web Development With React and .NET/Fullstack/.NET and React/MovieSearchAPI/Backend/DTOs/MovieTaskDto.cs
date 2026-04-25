namespace Backend.DTOs;

public class MovieTaskDto
{
    public int Id { get; set; }
    public string Title { get; set; } = string.Empty;
    public string? Description { get; set; }
    public string Category { get; set; } = string.Empty;
    public bool IsWatched { get; set; }
    public string? Year { get; set; }
    public string? PosterUrl { get; set; }
}
