FROM mcr.microsoft.com/dotnet/sdk:10.0 AS build
WORKDIR /src

COPY ["Backend/EventReminder.API/EventReminder.API.csproj", "Backend/EventReminder.API/"]
RUN dotnet restore "Backend/EventReminder.API/EventReminder.API.csproj"

COPY . .
WORKDIR "/src/Backend/EventReminder.API"
RUN dotnet publish "EventReminder.API.csproj" -c Release -o /app/publish /p:UseAppHost=false

FROM mcr.microsoft.com/dotnet/aspnet:10.0 AS final
WORKDIR /app
EXPOSE 8080

COPY --from=build /app/publish .
ENTRYPOINT ["dotnet", "EventReminder.API.dll"]
