using Unity.FPS.Game;
using Unity.FPS.Gameplay;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.FPS.UI
{
    public class CrosshairManager : MonoBehaviour
    {
        public Image CrosshairImage;
        public Sprite NullCrosshairSprite;
        public float CrosshairUpdateshrpness = 5f;

        [Header("Hit Confirmation")]
        [Tooltip("How long the crosshair should emphasize after damaging an enemy")]
        public float HitConfirmDuration = 0.12f;

        [Tooltip("Crosshair color briefly used after a successful hit")]
        public Color HitConfirmColor = Color.white;

        [Tooltip("Temporary crosshair scale multiplier used on hit")]
        public float HitConfirmScaleMultiplier = 1.18f;

        [Tooltip("How long the crosshair should emphasize after a killing blow")]
        public float KillConfirmDuration = 0.2f;

        [Tooltip("Crosshair color briefly used after a killing blow")]
        public Color KillConfirmColor = new Color(1f, 0.42f, 0.12f, 1f);

        [Tooltip("Temporary crosshair scale multiplier used on kill")]
        public float KillConfirmScaleMultiplier = 1.32f;

        PlayerWeaponsManager m_WeaponsManager;
        Health m_PlayerHealth;
        Actor m_PlayerActor;
        bool m_WasPointingAtEnemy;
        RectTransform m_CrosshairRectTransform;
        CrosshairData m_CrosshairDataDefault;
        CrosshairData m_CrosshairDataTarget;
        CrosshairData m_CurrentCrosshair;
        float m_HitConfirmStartTime = Mathf.NegativeInfinity;
        float m_HitConfirmEndTime = Mathf.NegativeInfinity;
        Color m_ActiveHitConfirmColor = Color.white;
        float m_ActiveHitConfirmScaleMultiplier = 1f;

        void Start()
        {
            m_WeaponsManager = FindFirstObjectByType<PlayerWeaponsManager>();
            DebugUtility.HandleErrorIfNullFindObject<PlayerWeaponsManager, CrosshairManager>(m_WeaponsManager, this);

            if (m_WeaponsManager != null)
            {
                m_PlayerHealth = m_WeaponsManager.GetComponent<Health>();
                m_PlayerActor = m_WeaponsManager.GetComponent<Actor>();
            }

            OnWeaponChanged(m_WeaponsManager.GetActiveWeapon());

            m_WeaponsManager.OnSwitchedToWeapon += OnWeaponChanged;
            EventManager.AddListener<DamageEvent>(OnDamageEvent);
        }

        void Update()
        {
            if (m_WeaponsManager == null)
                return;

            UpdateCrosshairPointingAtEnemy(false);
            m_WasPointingAtEnemy = m_WeaponsManager.IsPointingAtEnemy;
        }

        void UpdateCrosshairPointingAtEnemy(bool force)
        {
            if (m_WeaponsManager == null || CrosshairImage == null || m_CrosshairRectTransform == null)
                return;

            if (m_CrosshairDataDefault.CrosshairSprite == null)
                return;

            if ((force || !m_WasPointingAtEnemy) && m_WeaponsManager.IsPointingAtEnemy)
            {
                m_CurrentCrosshair = m_CrosshairDataTarget;
                CrosshairImage.sprite = m_CurrentCrosshair.CrosshairSprite;
                m_CrosshairRectTransform.sizeDelta = m_CurrentCrosshair.CrosshairSize * Vector2.one;
            }
            else if ((force || m_WasPointingAtEnemy) && !m_WeaponsManager.IsPointingAtEnemy)
            {
                m_CurrentCrosshair = m_CrosshairDataDefault;
                CrosshairImage.sprite = m_CurrentCrosshair.CrosshairSprite;
                m_CrosshairRectTransform.sizeDelta = m_CurrentCrosshair.CrosshairSize * Vector2.one;
            }

            Color targetColor = m_CurrentCrosshair.CrosshairColor;
            float targetSize = m_CurrentCrosshair.CrosshairSize;

            if (Time.unscaledTime < m_HitConfirmEndTime)
            {
                float duration = Mathf.Max(0.01f, m_HitConfirmEndTime - m_HitConfirmStartTime);
                float t = Mathf.Clamp01((Time.unscaledTime - m_HitConfirmStartTime) / duration);
                float pulse = 1f - t;
                targetColor = Color.Lerp(targetColor, m_ActiveHitConfirmColor, pulse);
                targetSize *= Mathf.Lerp(1f, m_ActiveHitConfirmScaleMultiplier, pulse);
            }

            m_CrosshairRectTransform.sizeDelta = Mathf.Lerp(m_CrosshairRectTransform.sizeDelta.x,
                targetSize,
                Time.unscaledDeltaTime * CrosshairUpdateshrpness) * Vector2.one;
            CrosshairImage.color = Color.Lerp(CrosshairImage.color, targetColor,
                Time.unscaledDeltaTime * CrosshairUpdateshrpness);
        }

        void OnWeaponChanged(WeaponController newWeapon)
        {
            if (newWeapon)
            {
                CrosshairImage.enabled = true;
                m_CrosshairDataDefault = newWeapon.CrosshairDataDefault;
                m_CrosshairDataTarget = newWeapon.CrosshairDataTargetInSight;
                m_CrosshairRectTransform = CrosshairImage.GetComponent<RectTransform>();
                DebugUtility.HandleErrorIfNullGetComponent<RectTransform, CrosshairManager>(m_CrosshairRectTransform,
                    this, CrosshairImage.gameObject);
            }
            else
            {
                if (NullCrosshairSprite)
                {
                    CrosshairImage.sprite = NullCrosshairSprite;
                }
                else
                {
                    CrosshairImage.enabled = false;
                }
            }

            UpdateCrosshairPointingAtEnemy(true);
        }

        void OnDamageEvent(DamageEvent evt)
        {
            if (m_WeaponsManager == null || evt == null)
                return;

            if (evt.Sender != m_WeaponsManager.gameObject)
                return;

            if (evt.Target == null || evt.Target == m_WeaponsManager.gameObject)
                return;

            Health targetHealth = evt.Target.GetComponent<Health>() ?? evt.Target.GetComponentInParent<Health>();
            if (targetHealth != null && targetHealth == m_PlayerHealth)
                return;

            Actor targetActor = evt.Target.GetComponent<Actor>() ?? evt.Target.GetComponentInParent<Actor>();
            if (targetActor != null && m_PlayerActor != null && targetActor.Affiliation == m_PlayerActor.Affiliation)
                return;

            TriggerHitConfirm(evt.IsFatal);
        }

        void TriggerHitConfirm(bool isFatal)
        {
            float duration = isFatal ? KillConfirmDuration : HitConfirmDuration;
            m_HitConfirmStartTime = Time.unscaledTime;
            m_HitConfirmEndTime = m_HitConfirmStartTime + Mathf.Max(0.01f, duration);
            m_ActiveHitConfirmColor = isFatal ? KillConfirmColor : HitConfirmColor;
            m_ActiveHitConfirmScaleMultiplier = Mathf.Max(1f,
                isFatal ? KillConfirmScaleMultiplier : HitConfirmScaleMultiplier);
        }

        void OnDestroy()
        {
            EventManager.RemoveListener<DamageEvent>(OnDamageEvent);

            if (m_WeaponsManager != null)
                m_WeaponsManager.OnSwitchedToWeapon -= OnWeaponChanged;
        }
    }
}