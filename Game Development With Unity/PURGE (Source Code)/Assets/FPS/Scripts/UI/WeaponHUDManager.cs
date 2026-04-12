using Unity.FPS.Game;
using Unity.FPS.Gameplay;
using UnityEngine;

namespace Unity.FPS.UI
{
    public class WeaponHUDManager : MonoBehaviour
    {
        [Tooltip("UI panel containing the layoutGroup for displaying weapon ammo")]
        public RectTransform AmmoPanel;

        [Tooltip("Prefab for displaying weapon ammo")]
        public GameObject AmmoCounterPrefab;

        PlayerWeaponsManager m_PlayerWeaponsManager;
        AmmoCounter m_ActiveAmmoCounter;

        void Start()
        {
            m_PlayerWeaponsManager = FindFirstObjectByType<PlayerWeaponsManager>();
            DebugUtility.HandleErrorIfNullFindObject<PlayerWeaponsManager, WeaponHUDManager>(m_PlayerWeaponsManager,
                this);

            m_PlayerWeaponsManager.OnAddedWeapon += AddWeapon;
            m_PlayerWeaponsManager.OnRemovedWeapon += RemoveWeapon;
            m_PlayerWeaponsManager.OnSwitchedToWeapon += ChangeWeapon;

            RefreshActiveWeaponDisplay();
        }

        void AddWeapon(WeaponController newWeapon, int weaponIndex)
        {
            RefreshActiveWeaponDisplay();
        }

        void RemoveWeapon(WeaponController newWeapon, int weaponIndex)
        {
            RefreshActiveWeaponDisplay();
        }

        void ChangeWeapon(WeaponController weapon)
        {
            RefreshActiveWeaponDisplay();
        }

        void RefreshActiveWeaponDisplay()
        {
            if (m_PlayerWeaponsManager == null)
                return;

            WeaponController activeWeapon = m_PlayerWeaponsManager.GetActiveWeapon();
            if (activeWeapon == null)
            {
                if (m_ActiveAmmoCounter != null)
                    m_ActiveAmmoCounter.gameObject.SetActive(false);
                return;
            }

            if (m_ActiveAmmoCounter == null)
            {
                GameObject ammoCounterInstance = Instantiate(AmmoCounterPrefab, AmmoPanel);
                m_ActiveAmmoCounter = ammoCounterInstance.GetComponent<AmmoCounter>();
                DebugUtility.HandleErrorIfNullGetComponent<AmmoCounter, WeaponHUDManager>(m_ActiveAmmoCounter, this,
                    ammoCounterInstance.gameObject);
            }

            m_ActiveAmmoCounter.gameObject.SetActive(true);
            m_ActiveAmmoCounter.Initialize(activeWeapon, m_PlayerWeaponsManager.ActiveWeaponIndex);

            if (AmmoPanel != null)
                UnityEngine.UI.LayoutRebuilder.ForceRebuildLayoutImmediate(AmmoPanel);
        }

        void OnDestroy()
        {
            if (m_PlayerWeaponsManager == null)
                return;

            m_PlayerWeaponsManager.OnAddedWeapon -= AddWeapon;
            m_PlayerWeaponsManager.OnRemovedWeapon -= RemoveWeapon;
            m_PlayerWeaponsManager.OnSwitchedToWeapon -= ChangeWeapon;
        }
    }
}