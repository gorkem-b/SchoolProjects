namespace Backend.DTOs;

public class MovieSearchItemDto
{
    public string ImdbId { get; set; } = string.Empty;
    public string Title { get; set; } = string.Empty;
    public string Year { get; set; } = string.Empty;
    public string Type { get; set; } = string.Empty;
    public string? PosterUrl { get; set; }
}
