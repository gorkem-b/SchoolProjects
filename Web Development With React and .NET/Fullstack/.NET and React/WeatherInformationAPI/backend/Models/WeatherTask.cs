namespace WeatherInformationAPI.Models;

public class WeatherTask
{
    public int Id { get; set; }
    public string Title { get; set; } = string.Empty;
    public string Description { get; set; } = string.Empty;
    public string Category { get; set; } = string.Empty;
    public string CityName { get; set; } = string.Empty;
    public WeatherTaskStatus Status { get; set; } = WeatherTaskStatus.Pending;
    public DateTime CreatedAt { get; set; } = DateTime.UtcNow;
}
