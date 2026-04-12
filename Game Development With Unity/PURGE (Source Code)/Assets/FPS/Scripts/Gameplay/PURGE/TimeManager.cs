using System.Collections.Generic;
using TMPro;
using Unity.FPS.Game;
using Unity.FPS.Gameplay;
using UnityEngine;

namespace PURGE.Gameplay
{
    public class TimeManager : MonoBehaviour
    {
        [Tooltip("Enable the countdown timer. Disable this for unlimited time.")]
        public bool EnableCountdown;

        [Tooltip("Countdown duration in seconds when the timer is enabled")]
        public float TimeRemaining = 300f;

        [Tooltip("Text element that displays the remaining time as MM:SS")]
        public TextMeshProUGUI TimerText;

        [Tooltip("Use unscaled time (optional, useful if you pause with timescale)")]
        public bool UseUnscaledTime;

        [Header("Urgency Feedback")]
        [Tooltip("When remaining time falls below this threshold, the timer enters warning mode")]
        public float WarningThresholdSeconds = 60f;

        [Tooltip("When remaining time falls below this threshold, the timer enters critical mode")]
        public float CriticalThresholdSeconds = 15f;

        [Tooltip("Color used for the timer in warning mode")]
        public Color WarningTimerColor = new Color(1f, 0.82f, 0.28f, 1f);

        [Tooltip("Color used for the timer in critical mode")]
        public Color CriticalTimerColor = new Color(1f, 0.28f, 0.22f, 1f);

        [Tooltip("How much the timer text scales while pulsing in urgency states")]
        public float UrgencyPulseScaleMultiplier = 1.08f;

        [Tooltip("Pulse speed while the timer is in critical state")]
        public float CriticalPulseFrequency = 6f;

        [Tooltip("Broadcasts toast messages at 60, 30 and 10 second thresholds")]
        public bool BroadcastUrgencyMessages = true;

        Health m_PlayerHealth;
        bool m_HasExpired;
        int m_LastDisplayedSeconds = -1;
        readonly HashSet<int> m_TriggeredUrgencyMessages = new HashSet<int>();
        Color m_BaseTimerColor = Color.white;
        Vector3 m_BaseTimerScale = Vector3.one;

        void Start()
        {
            var player = FindFirstObjectByType<PlayerCharacterController>();
            if (player != null)
            {
                m_PlayerHealth = player.GetComponent<Health>();
            }

            if (TimerText != null)
            {
                m_BaseTimerColor = TimerText.color;
                m_BaseTimerScale = TimerText.rectTransform.localScale;
            }

            UpdateTimerText();
        }

        void Update()
        {
            if (!EnableCountdown)
                return;

            if (m_HasExpired)
                return;

            float delta = UseUnscaledTime ? Time.unscaledDeltaTime : Time.deltaTime;
            TimeRemaining -= delta;

            if (TimeRemaining <= 0f)
            {
                TimeRemaining = 0f;
                m_HasExpired = true;
                BroadcastTimerMessage("TIME EXPIRED");
                UpdateTimerText();
                KillPlayer();
                return;
            }

            TryBroadcastUrgencyMessages();
            UpdateTimerText();
        }

        void KillPlayer()
        {
            if (m_PlayerHealth == null)
            {
                var player = FindFirstObjectByType<PlayerCharacterController>();
                if (player != null)
                {
                    m_PlayerHealth = player.GetComponent<Health>();
                }
            }

            if (m_PlayerHealth != null && m_PlayerHealth.CurrentHealth > 0f)
            {
                m_PlayerHealth.Kill();
            }
        }

        void UpdateTimerText()
        {
            if (TimerText == null)
                return;

            if (!EnableCountdown)
            {
                TimerText.text = string.Empty;
                ResetTimerVisuals();
                return;
            }

            int totalSeconds = Mathf.CeilToInt(Mathf.Max(0f, TimeRemaining));
            if (totalSeconds != m_LastDisplayedSeconds)
            {
                m_LastDisplayedSeconds = totalSeconds;
                int minutes = totalSeconds / 60;
                int seconds = totalSeconds % 60;
                TimerText.text = $"{minutes:00}:{seconds:00}";
            }

            UpdateTimerVisuals(totalSeconds);
        }

        void UpdateTimerVisuals(int totalSeconds)
        {
            if (TimerText == null)
                return;

            Color targetColor = m_BaseTimerColor;
            float pulseScale = 1f;

            if (totalSeconds <= CriticalThresholdSeconds)
            {
                targetColor = CriticalTimerColor;
                float pulse = 0.5f + 0.5f * Mathf.Sin(Time.unscaledTime * CriticalPulseFrequency);
                pulseScale = Mathf.Lerp(1f, UrgencyPulseScaleMultiplier, pulse);
            }
            else if (totalSeconds <= WarningThresholdSeconds)
            {
                targetColor = WarningTimerColor;
                float pulse = 0.5f + 0.5f * Mathf.Sin(Time.unscaledTime * (CriticalPulseFrequency * 0.45f));
                pulseScale = Mathf.Lerp(1f, 1f + (UrgencyPulseScaleMultiplier - 1f) * 0.45f, pulse);
            }

            TimerText.color = targetColor;
            TimerText.rectTransform.localScale = m_BaseTimerScale * pulseScale;
        }

        void ResetTimerVisuals()
        {
            if (TimerText == null)
                return;

            TimerText.color = m_BaseTimerColor;
            TimerText.rectTransform.localScale = m_BaseTimerScale;
        }

        void TryBroadcastUrgencyMessages()
        {
            if (!BroadcastUrgencyMessages)
                return;

            int totalSeconds = Mathf.CeilToInt(Mathf.Max(0f, TimeRemaining));
            TryBroadcastUrgencyMessageAtThreshold(totalSeconds, 60, "1 MINUTE REMAINING");
            TryBroadcastUrgencyMessageAtThreshold(totalSeconds, 30, "30 SECONDS REMAINING");
            TryBroadcastUrgencyMessageAtThreshold(totalSeconds, 10, "10 SECONDS REMAINING");
        }

        void TryBroadcastUrgencyMessageAtThreshold(int totalSeconds, int thresholdSeconds, string message)
        {
            if (totalSeconds > thresholdSeconds || m_TriggeredUrgencyMessages.Contains(thresholdSeconds))
                return;

            m_TriggeredUrgencyMessages.Add(thresholdSeconds);
            BroadcastTimerMessage(message);
        }

        void BroadcastTimerMessage(string message)
        {
            if (string.IsNullOrEmpty(message))
                return;

            DisplayMessageEvent displayMessage = Events.DisplayMessageEvent;
            displayMessage.Message = message;
            displayMessage.DelayBeforeDisplay = 0f;
            EventManager.Broadcast(displayMessage);
        }
    }
}
