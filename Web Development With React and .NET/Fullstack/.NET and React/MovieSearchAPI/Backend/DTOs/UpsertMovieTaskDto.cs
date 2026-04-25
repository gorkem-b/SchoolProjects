using System.ComponentModel.DataAnnotations;

namespace Backend.DTOs;

public class UpsertMovieTaskDto
{
    [Required]
    [StringLength(200)]
    public string Title { get; set; } = string.Empty;

    [StringLength(1000)]
    public string? Description { get; set; }

    [Required]
    [StringLength(50)]
    public string Category { get; set; } = "Watchlist";

    public bool IsWatched { get; set; }

    [StringLength(10)]
    public string? Year { get; set; }

    [StringLength(500)]
    public string? PosterUrl { get; set; }
}
