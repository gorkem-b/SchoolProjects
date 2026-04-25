using System.ComponentModel.DataAnnotations;

namespace BookLibrary.Core.DTOs.Books;

public class CreateBookDto
{
    [Required]
    [MaxLength(200)]
    public string Title { get; set; } = string.Empty;

    [Required]
    [MaxLength(150)]
    public string Author { get; set; } = string.Empty;

    [Required]
    [MaxLength(100)]
    public string Genre { get; set; } = string.Empty;
}