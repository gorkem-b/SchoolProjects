using System;
using System.Collections.Generic;
using Unity.FPS.Game;
using Unity.FPS.Gameplay;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.FPS.UI
{
    public class NotificationHUDManager : MonoBehaviour
    {
        [Tooltip("UI panel containing the layoutGroup for displaying notifications")]
        public RectTransform NotificationPanel;

        [Tooltip("Prefab for the notifications")]
        public GameObject NotificationPrefab;

        [Tooltip("Extra visible time added when the same notification is triggered again")]
        public float DuplicateNotificationRefreshExtraTime = 0.45f;

        [Tooltip("Suppress identical notifications if they repeat too quickly after disappearing")]
        public float DuplicateNotificationCooldown = 1.25f;

        PlayerWeaponsManager m_PlayerWeaponsManager;
        readonly Dictionary<string, float> m_LastNotificationTimes = new Dictionary<string, float>();

        void Awake()
        {
            m_PlayerWeaponsManager = FindFirstObjectByType<PlayerWeaponsManager>();
            DebugUtility.HandleErrorIfNullFindObject<PlayerWeaponsManager, NotificationHUDManager>(m_PlayerWeaponsManager,
                this);
            m_PlayerWeaponsManager.OnAddedWeapon += OnPickupWeapon;

            EventManager.AddListener<ObjectiveUpdateEvent>(OnObjectiveUpdateEvent);
        }

        void OnObjectiveUpdateEvent(ObjectiveUpdateEvent evt)
        {
            if (!string.IsNullOrEmpty(evt.NotificationText))
                CreateNotification(evt.NotificationText);
        }

        void OnPickupWeapon(WeaponController weaponController, int index)
        {
            if (index != 0)
                CreateNotification("Picked up weapon : " + weaponController.WeaponName);
        }

        public void CreateNotification(string text)
        {
            if (NotificationPanel == null || NotificationPrefab == null)
                return;

            string normalizedText = NormalizeNotificationText(text);
            if (string.IsNullOrWhiteSpace(normalizedText))
                return;

            float currentTime = Time.unscaledTime;

            for (int i = 0; i < NotificationPanel.childCount; i++)
            {
                NotificationToast existingToast = NotificationPanel.GetChild(i).GetComponent<NotificationToast>();
                if (existingToast == null || !existingToast.MatchesText(normalizedText))
                    continue;

                existingToast.ExtendAndPulse(DuplicateNotificationRefreshExtraTime);
                existingToast.transform.SetSiblingIndex(0);
                m_LastNotificationTimes[normalizedText] = currentTime;
                LayoutRebuilder.ForceRebuildLayoutImmediate(NotificationPanel);
                return;
            }

            if (m_LastNotificationTimes.TryGetValue(normalizedText, out float lastShownTime)
                && currentTime < lastShownTime + DuplicateNotificationCooldown)
                return;

            GameObject notificationInstance = Instantiate(NotificationPrefab, NotificationPanel);
            notificationInstance.transform.SetSiblingIndex(0);

            NotificationToast toast = notificationInstance.GetComponent<NotificationToast>();
            if (toast)
            {
                toast.Initialize(normalizedText);
            }

            m_LastNotificationTimes[normalizedText] = currentTime;
            LayoutRebuilder.ForceRebuildLayoutImmediate(NotificationPanel);
        }

        static string NormalizeNotificationText(string text)
        {
            if (string.IsNullOrWhiteSpace(text))
                return string.Empty;

            return string.Join(" ", text.Split((char[]) null, StringSplitOptions.RemoveEmptyEntries));
        }

        void OnDestroy()
        {
            EventManager.RemoveListener<ObjectiveUpdateEvent>(OnObjectiveUpdateEvent);
            if (m_PlayerWeaponsManager != null)
                m_PlayerWeaponsManager.OnAddedWeapon -= OnPickupWeapon;
        }
    }
}