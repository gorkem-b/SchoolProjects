using System.ComponentModel.DataAnnotations;
using WeatherInformationAPI.Models;

namespace WeatherInformationAPI.DTOs;

public class TaskUpdateDto
{
    [Required]
    [MaxLength(100)]
    public string Title { get; set; } = string.Empty;

    [MaxLength(500)]
    public string? Description { get; set; }

    [Required]
    [MaxLength(50)]
    public string Category { get; set; } = string.Empty;

    [Required]
    [MaxLength(100)]
    public string CityName { get; set; } = string.Empty;

    [Required]
    public WeatherTaskStatus Status { get; set; }
}
