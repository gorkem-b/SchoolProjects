using PURGE.Gameplay;
using Unity.FPS.Game;
using Unity.FPS.Gameplay;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.FPS.UI
{
    public class FeedbackFlashHUD : MonoBehaviour
    {
        [Header("References")] [Tooltip("Image component of the flash")]
        public Image FlashImage;

        [Tooltip("CanvasGroup to fade the damage flash, used when recieving damage end healing")]
        public CanvasGroup FlashCanvasGroup;

        [Tooltip("CanvasGroup to fade the critical health vignette")]
        public CanvasGroup VignetteCanvasGroup;

        [Header("Damage")] [Tooltip("Color of the damage flash")]
        public Color DamageFlashColor;

        [Tooltip("Duration of the damage flash")]
        public float DamageFlashDuration;

        [Tooltip("Max alpha of the damage flash")]
        public float DamageFlashMaxAlpha = 1f;

        [Header("Critical health")] [Tooltip("Max alpha of the critical vignette")]
        public float CriticaHealthVignetteMaxAlpha = .8f;

        [Tooltip("Frequency at which the vignette will pulse when at critical health")]
        public float PulsatingVignetteFrequency = 4f;

        [Header("Heal")] [Tooltip("Color of the heal flash")]
        public Color HealFlashColor;

        [Tooltip("Duration of the heal flash")]
        public float HealFlashDuration;

        [Tooltip("Max alpha of the heal flash")]
        public float HealFlashMaxAlpha = 1f;

        [Header("Boss threat")]
        [Tooltip("Color used when receiving damage from the assault mech boss")]
        public Color BossDamageFlashColor = new Color(1f, 0.45f, 0.12f, 1f);

        [Tooltip("Duration of the boss damage flash")]
        public float BossDamageFlashDuration = 0.32f;

        [Tooltip("Max alpha of the boss damage flash")]
        public float BossDamageFlashMaxAlpha = 1f;

        bool m_FlashActive;
        float m_LastTimeFlashStarted = Mathf.NegativeInfinity;
        float m_CurrentFlashDuration;
        float m_CurrentFlashMaxAlpha;
        Health m_PlayerHealth;
        GameFlowManager m_GameFlowManager;

        void Start()
        {
            // Subscribe to player damage events
            PlayerCharacterController playerCharacterController = FindFirstObjectByType<PlayerCharacterController>();
            DebugUtility.HandleErrorIfNullFindObject<PlayerCharacterController, FeedbackFlashHUD>(
                playerCharacterController, this);

            m_PlayerHealth = playerCharacterController.GetComponent<Health>();
            DebugUtility.HandleErrorIfNullGetComponent<Health, FeedbackFlashHUD>(m_PlayerHealth, this,
                playerCharacterController.gameObject);

            m_GameFlowManager = FindFirstObjectByType<GameFlowManager>();
            DebugUtility.HandleErrorIfNullFindObject<GameFlowManager, FeedbackFlashHUD>(m_GameFlowManager, this);

            m_PlayerHealth.OnDamaged += OnTakeDamage;
            m_PlayerHealth.OnHealed += OnHealed;

            m_CurrentFlashDuration = DamageFlashDuration;
            m_CurrentFlashMaxAlpha = DamageFlashMaxAlpha;
        }

        void Update()
        {
            if (m_PlayerHealth == null)
                return;

            if (m_PlayerHealth.IsCritical())
            {
                VignetteCanvasGroup.gameObject.SetActive(true);
                float vignetteAlpha =
                    (1 - (m_PlayerHealth.CurrentHealth / m_PlayerHealth.MaxHealth /
                          m_PlayerHealth.CriticalHealthRatio)) * CriticaHealthVignetteMaxAlpha;

                if (m_GameFlowManager.GameIsEnding)
                    VignetteCanvasGroup.alpha = vignetteAlpha;
                else
                    VignetteCanvasGroup.alpha =
                        ((Mathf.Sin(Time.unscaledTime * PulsatingVignetteFrequency) / 2) + 0.5f) * vignetteAlpha;
            }
            else
            {
                VignetteCanvasGroup.gameObject.SetActive(false);
            }


            if (m_FlashActive)
            {
                float normalizedTimeSinceDamage = (Time.unscaledTime - m_LastTimeFlashStarted) / Mathf.Max(0.01f, m_CurrentFlashDuration);

                if (normalizedTimeSinceDamage < 1f)
                {
                    float flashAmount = m_CurrentFlashMaxAlpha * (1f - normalizedTimeSinceDamage);
                    FlashCanvasGroup.alpha = flashAmount;
                }
                else
                {
                    FlashCanvasGroup.gameObject.SetActive(false);
                    m_FlashActive = false;
                }
            }
        }

        void ResetFlash(float duration, float maxAlpha)
        {
            m_LastTimeFlashStarted = Time.unscaledTime;
            m_FlashActive = true;
            m_CurrentFlashDuration = Mathf.Max(0.01f, duration);
            m_CurrentFlashMaxAlpha = maxAlpha;
            FlashCanvasGroup.alpha = 0f;
            FlashCanvasGroup.gameObject.SetActive(true);
        }

        void OnTakeDamage(float dmg, GameObject damageSource)
        {
            bool isBossThreat = damageSource != null && damageSource.GetComponentInParent<AssaultMechBoss>() != null;
            FlashImage.color = isBossThreat ? BossDamageFlashColor : DamageFlashColor;
            ResetFlash(isBossThreat ? BossDamageFlashDuration : DamageFlashDuration,
                isBossThreat ? BossDamageFlashMaxAlpha : DamageFlashMaxAlpha);
        }

        void OnHealed(float amount)
        {
            FlashImage.color = HealFlashColor;
            ResetFlash(HealFlashDuration, HealFlashMaxAlpha);
        }

        void OnDestroy()
        {
            if (m_PlayerHealth != null)
            {
                m_PlayerHealth.OnDamaged -= OnTakeDamage;
                m_PlayerHealth.OnHealed -= OnHealed;
            }
        }
    }
}