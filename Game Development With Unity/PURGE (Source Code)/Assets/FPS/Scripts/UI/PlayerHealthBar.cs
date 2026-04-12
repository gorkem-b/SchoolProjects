using Unity.FPS.Game;
using Unity.FPS.Gameplay;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.FPS.UI
{
    public class PlayerHealthBar : MonoBehaviour
    {
        [Tooltip("Image component dispplaying current health")]
        public Image HealthFillImage;

        Health m_PlayerHealth;

        void Awake()
        {
            DisablePurgeUnusedHudElements();
        }

        void Start()
        {
            PlayerCharacterController playerCharacterController =
                GameObject.FindFirstObjectByType<PlayerCharacterController>();
            DebugUtility.HandleErrorIfNullFindObject<PlayerCharacterController, PlayerHealthBar>(
                playerCharacterController, this);

            m_PlayerHealth = playerCharacterController.GetComponent<Health>();
            DebugUtility.HandleErrorIfNullGetComponent<Health, PlayerHealthBar>(m_PlayerHealth, this,
                playerCharacterController.gameObject);
        }

        void Update()
        {
            // update health bar value
            HealthFillImage.fillAmount = m_PlayerHealth.CurrentHealth / m_PlayerHealth.MaxHealth;
        }

        void DisablePurgeUnusedHudElements()
        {
            Transform compass = transform.Find("HUD/Compass");
            if (compass != null)
                compass.gameObject.SetActive(false);
        }
    }
}