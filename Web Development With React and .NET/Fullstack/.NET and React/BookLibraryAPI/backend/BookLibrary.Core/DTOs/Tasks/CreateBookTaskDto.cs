using BookLibrary.Core.Enums;
using System.ComponentModel.DataAnnotations;

namespace BookLibrary.Core.DTOs.Tasks;

public class CreateBookTaskDto
{
    [Required]
    [MaxLength(200)]
    public string Title { get; set; } = string.Empty;

    public BookTaskStatus Status { get; set; } = BookTaskStatus.ToRead;

    [Range(1, int.MaxValue)]
    public int BookId { get; set; }
}