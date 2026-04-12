using Unity.FPS.Game;
using Unity.FPS.Gameplay;
using UnityEngine;

namespace PURGE.Gameplay
{
    [RequireComponent(typeof(Collider))]
    public class Pickup : MonoBehaviour
    {
        [Header("Pickup")]
        [Tooltip("When collected, heals player to full health")]
        public bool FullHealOnPickup = true;

        [Header("Feedback")]
        public AudioClip PickupSfx;
        public GameObject PickupVfxPrefab;

        [Header("Readability")]
        [Tooltip("Idle hover amplitude for making pickups easier to notice")]
        public float FloatAmplitude = 0.14f;

        [Tooltip("Idle hover frequency")]
        public float FloatFrequency = 1.6f;

        [Tooltip("Continuous Y-axis rotation speed")]
        public float RotationSpeed = 65f;

        [Tooltip("Scale pulse multiplier applied on top of the base scale")]
        public float ScalePulseMultiplier = 1.05f;

        [Tooltip("Scale pulse frequency")]
        public float ScalePulseFrequency = 2.1f;

        [Tooltip("If true, the pickup won't be consumed while the player is already at full health")]
        public bool RequireMissingHealth = true;

        [Tooltip("Optional toast text shown if the player tries to take the pickup at full health")]
        public string FullHealthMessage = "Health already full";

        [Tooltip("Minimum time between full-health reminder toasts")]
        public float FullHealthMessageCooldown = 1.5f;

        bool m_IsConsumed;
        Vector3 m_BaseLocalPosition;
        Quaternion m_BaseLocalRotation;
        Vector3 m_BaseLocalScale;
        float m_AnimationOffset;
        float m_LastRejectedMessageTime = Mathf.NegativeInfinity;

        void Awake()
        {
            CacheBaseTransform();
            m_AnimationOffset = Random.Range(0f, 100f);
        }

        void Start()
        {
            CacheBaseTransform();
        }

        void OnValidate()
        {
            var col = GetComponent<Collider>();
            if (col != null)
            {
                col.isTrigger = true;
            }
        }

        void Update()
        {
            if (m_IsConsumed)
                return;

            float t = Time.time + m_AnimationOffset;
            float hover = Mathf.Sin(t * FloatFrequency) * FloatAmplitude;
            float scalePulse = 1f + (Mathf.Sin(t * ScalePulseFrequency) * 0.5f + 0.5f) * (Mathf.Max(1f, ScalePulseMultiplier) - 1f);

            transform.localPosition = m_BaseLocalPosition + Vector3.up * hover;
            transform.localRotation = m_BaseLocalRotation * Quaternion.Euler(0f, t * RotationSpeed, 0f);
            transform.localScale = m_BaseLocalScale * scalePulse;
        }

        void OnTriggerEnter(Collider other)
        {
            if (m_IsConsumed)
                return;

            PlayerCharacterController player = other.GetComponentInParent<PlayerCharacterController>();
            if (player == null || !player.CompareTag("Player"))
                return;

            if (RequireMissingHealth && FullHealOnPickup)
            {
                Health health = player.GetComponent<Health>();
                if (health != null && !health.CanPickup())
                {
                    TryBroadcastFullHealthMessage();
                    return;
                }
            }

            ApplyPickup(player);
            PlayPickupFeedback();
            m_IsConsumed = true;
            Destroy(gameObject);
        }

        void ApplyPickup(PlayerCharacterController player)
        {
            if (!FullHealOnPickup)
                return;

            var health = player.GetComponent<Health>();
            if (health != null)
            {
                health.Heal(health.MaxHealth);
            }
        }

        void PlayPickupFeedback()
        {
            if (PickupSfx != null)
            {
                AudioUtility.CreateSFX(PickupSfx, transform.position, AudioUtility.AudioGroups.Pickup, 0f);
            }

            if (PickupVfxPrefab != null)
            {
                var vfx = Instantiate(PickupVfxPrefab, transform.position, Quaternion.identity);
                Destroy(vfx, 5f);
            }
        }

        void TryBroadcastFullHealthMessage()
        {
            if (string.IsNullOrEmpty(FullHealthMessage))
                return;

            if (Time.time < m_LastRejectedMessageTime + Mathf.Max(0.1f, FullHealthMessageCooldown))
                return;

            m_LastRejectedMessageTime = Time.time;
            DisplayMessageEvent displayMessage = Events.DisplayMessageEvent;
            displayMessage.Message = FullHealthMessage;
            displayMessage.DelayBeforeDisplay = 0f;
            EventManager.Broadcast(displayMessage);
        }

        void CacheBaseTransform()
        {
            m_BaseLocalPosition = transform.localPosition;
            m_BaseLocalRotation = transform.localRotation;
            m_BaseLocalScale = transform.localScale;
        }

    }
}
