using System.Collections.Generic;
using Unity.FPS.Game;
using UnityEngine;

namespace Unity.FPS.UI
{
    public class DisplayMessageManager : MonoBehaviour
    {
        const string NarrativePrefix = "[NARRATIVE]";

        public UITable DisplayMessageRect;
        public RectTransform NarrativeMessageRect;
        public NotificationToast MessagePrefab;
        public NotificationToast NarrativeMessagePrefab;

        [Tooltip("Maximum number of display messages visible at the same time")]
        public int MaxVisibleMessages = 2;

        [Tooltip("Maximum number of narrative subtitle messages visible at the same time")]
        public int MaxVisibleNarrativeMessages = 1;

        [Tooltip("Minimum delay between starting two messages")]
        public float MinTimeBetweenMessages = 0.1f;

        [Tooltip("Suppresses identical messages if they repeat too quickly")]
        public float DuplicateMessageCooldown = 1.25f;

        [Tooltip("Extra visible time added when a duplicate refreshes an active message")]
        public float DuplicateRefreshExtraTime = 0.7f;

        struct PendingMessage
        {
            public float ReadyTime;
            public string Message;
            public bool IsNarrative;
        }

        List<PendingMessage> m_PendingMessages;
        List<NotificationToast> m_ActiveMessages;
        Dictionary<string, float> m_LastMessageTimes;
        float m_LastDisplayTime = Mathf.NegativeInfinity;

        void Awake()
        {
            EventManager.AddListener<DisplayMessageEvent>(OnDisplayMessageEvent);
            m_PendingMessages = new List<PendingMessage>();
            m_ActiveMessages = new List<NotificationToast>();
            m_LastMessageTimes = new Dictionary<string, float>();
        }

        void OnDisplayMessageEvent(DisplayMessageEvent evt)
        {
            string rawMessage = evt.Message != null ? evt.Message.Trim() : string.Empty;
            if (string.IsNullOrEmpty(rawMessage))
                return;

            bool isNarrativeMessage = IsNarrativeMessage(rawMessage);
            string message = NormalizeMessage(rawMessage);
            if (string.IsNullOrEmpty(message))
                return;

            float currentTime = Time.unscaledTime;

            CleanupActiveMessages();

            if (TryRefreshActiveMessage(message, isNarrativeMessage))
                return;

            if (IsPendingDuplicate(message, isNarrativeMessage))
                return;

            string messageKey = GetMessageKey(message, isNarrativeMessage);
            if (m_LastMessageTimes.TryGetValue(messageKey, out float lastShownTime)
                && currentTime < lastShownTime + DuplicateMessageCooldown)
                return;

            m_PendingMessages.Add(new PendingMessage
            {
                ReadyTime = currentTime + Mathf.Max(0f, evt.DelayBeforeDisplay),
                Message = message,
                IsNarrative = isNarrativeMessage,
            });
        }

        void Update()
        {
            CleanupActiveMessages();

            float currentTime = Time.unscaledTime;

            if (m_PendingMessages.Count == 0)
                return;

            if (currentTime < m_LastDisplayTime + Mathf.Max(0f, MinTimeBetweenMessages))
                return;

            for (int i = 0; i < m_PendingMessages.Count; i++)
            {
                PendingMessage message = m_PendingMessages[i];
                if (currentTime < message.ReadyTime)
                    continue;

                if (GetActiveMessageCount(message.IsNarrative) >= GetMaxVisibleCount(message.IsNarrative))
                    continue;

                if (TryRefreshActiveMessage(message.Message, message.IsNarrative))
                {
                    m_PendingMessages.RemoveAt(i);
                    return;
                }

                DisplayMessage(message.Message, message.IsNarrative);
                m_PendingMessages.RemoveAt(i);
                return;
            }
        }

        void DisplayMessage(string message, bool isNarrativeMessage)
        {
            Transform parent = ResolveParent(isNarrativeMessage);
            NotificationToast sourcePrefab = isNarrativeMessage && NarrativeMessagePrefab != null
                ? NarrativeMessagePrefab
                : MessagePrefab;

            NotificationToast notification = Instantiate(sourcePrefab, parent).GetComponent<NotificationToast>();
            notification.Initialize(message, isNarrativeMessage);

            if (!isNarrativeMessage && DisplayMessageRect != null)
                DisplayMessageRect.UpdateTable(notification.gameObject);

            m_ActiveMessages.Add(notification);
            m_LastDisplayTime = Time.unscaledTime;
            m_LastMessageTimes[GetMessageKey(message, isNarrativeMessage)] = Time.unscaledTime;
        }

        Transform ResolveParent(bool isNarrativeMessage)
        {
            if (isNarrativeMessage)
            {
                if (NarrativeMessageRect != null)
                    return NarrativeMessageRect;

                RectTransform ownRect = transform as RectTransform;
                if (ownRect != null)
                    return ownRect;
            }

            if (DisplayMessageRect != null)
                return DisplayMessageRect.transform;

            return transform;
        }

        int GetActiveMessageCount(bool narrativeMessages)
        {
            int count = 0;
            for (int i = 0; i < m_ActiveMessages.Count; i++)
            {
                NotificationToast toast = m_ActiveMessages[i];
                if (toast == null || toast.UsesNarrativeStyle != narrativeMessages)
                    continue;

                count++;
            }

            return count;
        }

        int GetMaxVisibleCount(bool narrativeMessages)
        {
            return Mathf.Max(1, narrativeMessages ? MaxVisibleNarrativeMessages : MaxVisibleMessages);
        }

        static bool IsNarrativeMessage(string message)
        {
            return !string.IsNullOrEmpty(message)
                && (message.StartsWith(NarrativePrefix)
                    || message.StartsWith("COMMAND //")
                    || message.StartsWith("SYSTEM //")
                    || message.StartsWith("LOG //"));
        }

        static string NormalizeMessage(string message)
        {
            if (string.IsNullOrEmpty(message))
                return string.Empty;

            string normalized = message.Trim();
            if (normalized.StartsWith(NarrativePrefix))
                normalized = normalized.Substring(NarrativePrefix.Length).Trim();

            if (normalized.StartsWith("COMMAND //"))
                normalized = normalized.Substring("COMMAND //".Length).Trim();
            else if (normalized.StartsWith("SYSTEM //"))
                normalized = normalized.Substring("SYSTEM //".Length).Trim();
            else if (normalized.StartsWith("LOG //"))
                normalized = normalized.Substring("LOG //".Length).Trim();

            return normalized;
        }

        static string GetMessageKey(string message, bool isNarrativeMessage)
        {
            return (isNarrativeMessage ? "N|" : "S|") + message;
        }

        bool TryRefreshActiveMessage(string message, bool isNarrativeMessage)
        {
            for (int i = 0; i < m_ActiveMessages.Count; i++)
            {
                NotificationToast toast = m_ActiveMessages[i];
                if (toast == null)
                    continue;

                if (toast.UsesNarrativeStyle != isNarrativeMessage)
                    continue;

                if (!toast.MatchesText(message))
                    continue;

                toast.ExtendAndPulse(DuplicateRefreshExtraTime);
                toast.transform.SetSiblingIndex(0);
                m_LastMessageTimes[GetMessageKey(message, isNarrativeMessage)] = Time.unscaledTime;
                return true;
            }

            return false;
        }

        bool IsPendingDuplicate(string message, bool isNarrativeMessage)
        {
            for (int i = 0; i < m_PendingMessages.Count; i++)
            {
                if (m_PendingMessages[i].Message == message && m_PendingMessages[i].IsNarrative == isNarrativeMessage)
                    return true;
            }

            return false;
        }

        void CleanupActiveMessages()
        {
            m_ActiveMessages.RemoveAll(x => x == null);
        }

        void OnDestroy()
        {
            EventManager.RemoveListener<DisplayMessageEvent>(OnDisplayMessageEvent);
        }
    }
}