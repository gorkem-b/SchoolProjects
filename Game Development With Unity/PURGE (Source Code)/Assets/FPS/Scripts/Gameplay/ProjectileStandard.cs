using System.Collections.Generic;
using Unity.FPS.Game;
using UnityEngine;

namespace Unity.FPS.Gameplay
{
    public class ProjectileStandard : ProjectileBase
    {
        [Header("General")] [Tooltip("Radius of this projectile's collision detection")]
        public float Radius = 0.01f;

        [Tooltip("Transform representing the root of the projectile (used for accurate collision detection)")]
        public Transform Root;

        [Tooltip("Transform representing the tip of the projectile (used for accurate collision detection)")]
        public Transform Tip;

        [Tooltip("LifeTime of the projectile")]
        public float MaxLifeTime = 5f;

        [Tooltip("VFX prefab to spawn upon impact")]
        public GameObject ImpactVfx;

        [Tooltip("LifeTime of the VFX before being destroyed")]
        public float ImpactVfxLifetime = 5f;

        [Tooltip("Offset along the hit normal where the VFX will be spawned")]
        public float ImpactVfxSpawnOffset = 0.1f;

        [Tooltip("Clip to play on impact")] 
        public AudioClip ImpactSfxClip;

        [Tooltip("Layers this projectile can collide with")]
        public LayerMask HittableLayers = -1;

        [Header("Movement")] [Tooltip("Speed of the projectile")]
        public float Speed = 20f;

        [Tooltip("Downward acceleration from gravity")]
        public float GravityDownAcceleration = 0f;

        [Tooltip(
            "Distance over which the projectile will correct its course to fit the intended trajectory (used to drift projectiles towards center of screen in First Person view). At values under 0, there is no correction")]
        public float TrajectoryCorrectionDistance = -1;

        [Tooltip("Determines if the projectile inherits the velocity that the weapon's muzzle had when firing")]
        public bool InheritWeaponVelocity = false;

        [Header("Damage")] [Tooltip("Damage of the projectile")]
        public float Damage = 40f;

        [Tooltip("Area of damage. Keep empty if you don<t want area damage")]
        public DamageArea AreaOfDamage;

        [Header("Debug")] [Tooltip("Color of the projectile radius debug view")]
        public Color RadiusColor = Color.cyan * 0.2f;

        [Header("Trail")]
        [Tooltip("Adds a short readable trail behind the projectile")]
        public bool UseTravelTrail;

        [Tooltip("How long the projectile trail remains visible")]
        public float TrailDuration = 0.16f;

        [Tooltip("Minimum distance required before a new trail point is added")]
        public float TrailMinVertexDistance = 0.02f;

        [Tooltip("Width of the trail near the projectile")]
        public float TrailStartWidth = 0.12f;

        [Tooltip("Width of the trail tail")]
        public float TrailEndWidth = 0.03f;

        [ColorUsage(true, true)]
        [Tooltip("Color of the trail close to the projectile")]
        public Color TrailStartColor = new Color(0.35f, 0.95f, 1f, 1f);

        [ColorUsage(true, true)]
        [Tooltip("Color of the trail tail")]
        public Color TrailEndColor = new Color(0.08f, 0.45f, 1f, 0f);

        [Tooltip("Optional material override for the projectile trail")]
        public Material TrailMaterial;

        ProjectileBase m_ProjectileBase;
        Vector3 m_LastRootPosition;
        Vector3 m_Velocity;
        bool m_HasTrajectoryOverride;
        float m_ShootTime;
        Vector3 m_TrajectoryCorrectionVector;
        Vector3 m_ConsumedTrajectoryCorrectionVector;
        List<Collider> m_IgnoredColliders;
        TrailRenderer m_TrailRenderer;
        bool m_TrailDetached;

        const QueryTriggerInteraction k_TriggerInteraction = QueryTriggerInteraction.Collide;

        void OnEnable()
        {
            m_ProjectileBase = GetComponent<ProjectileBase>();
            DebugUtility.HandleErrorIfNullGetComponent<ProjectileBase, ProjectileStandard>(m_ProjectileBase, this,
                gameObject);

            m_ProjectileBase.OnShoot += OnShoot;

            EnsureTrailRenderer();

            Destroy(gameObject, MaxLifeTime);
        }

        new void OnShoot()
        {
            m_ShootTime = Time.time;
            m_LastRootPosition = Root.position;
            m_Velocity = transform.forward * Speed;
            m_IgnoredColliders = new List<Collider>();
            transform.position += m_ProjectileBase.InheritedMuzzleVelocity * Time.deltaTime;

            // Ignore colliders of owner
            Collider[] ownerColliders = m_ProjectileBase.Owner.GetComponentsInChildren<Collider>();
            m_IgnoredColliders.AddRange(ownerColliders);

            ResetTrail();

            // Handle case of player shooting (make projectiles not go through walls, and remember center-of-screen trajectory)
            PlayerWeaponsManager playerWeaponsManager = m_ProjectileBase.Owner.GetComponent<PlayerWeaponsManager>();
            if (playerWeaponsManager)
            {
                m_HasTrajectoryOverride = true;

                Vector3 cameraToMuzzle = (m_ProjectileBase.InitialPosition -
                                          playerWeaponsManager.WeaponCamera.transform.position);

                m_TrajectoryCorrectionVector = Vector3.ProjectOnPlane(-cameraToMuzzle,
                    playerWeaponsManager.WeaponCamera.transform.forward);
                if (TrajectoryCorrectionDistance == 0)
                {
                    transform.position += m_TrajectoryCorrectionVector;
                    m_ConsumedTrajectoryCorrectionVector = m_TrajectoryCorrectionVector;
                }
                else if (TrajectoryCorrectionDistance < 0)
                {
                    m_HasTrajectoryOverride = false;
                }

                if (Physics.Raycast(playerWeaponsManager.WeaponCamera.transform.position, cameraToMuzzle.normalized,
                    out RaycastHit hit, cameraToMuzzle.magnitude, HittableLayers, k_TriggerInteraction))
                {
                    if (IsHitValid(hit))
                    {
                        OnHit(hit.point, hit.normal, hit.collider);
                    }
                }
            }
        }

        void Update()
        {
            // Move
            transform.position += m_Velocity * Time.deltaTime;
            if (InheritWeaponVelocity)
            {
                transform.position += m_ProjectileBase.InheritedMuzzleVelocity * Time.deltaTime;
            }

            // Drift towards trajectory override (this is so that projectiles can be centered 
            // with the camera center even though the actual weapon is offset)
            if (m_HasTrajectoryOverride && m_ConsumedTrajectoryCorrectionVector.sqrMagnitude <
                m_TrajectoryCorrectionVector.sqrMagnitude)
            {
                Vector3 correctionLeft = m_TrajectoryCorrectionVector - m_ConsumedTrajectoryCorrectionVector;
                float distanceThisFrame = (Root.position - m_LastRootPosition).magnitude;
                Vector3 correctionThisFrame =
                    (distanceThisFrame / TrajectoryCorrectionDistance) * m_TrajectoryCorrectionVector;
                correctionThisFrame = Vector3.ClampMagnitude(correctionThisFrame, correctionLeft.magnitude);
                m_ConsumedTrajectoryCorrectionVector += correctionThisFrame;

                // Detect end of correction
                if (m_ConsumedTrajectoryCorrectionVector.sqrMagnitude == m_TrajectoryCorrectionVector.sqrMagnitude)
                {
                    m_HasTrajectoryOverride = false;
                }

                transform.position += correctionThisFrame;
            }

            // Orient towards velocity
            transform.forward = m_Velocity.normalized;

            // Gravity
            if (GravityDownAcceleration > 0)
            {
                // add gravity to the projectile velocity for ballistic effect
                m_Velocity += Vector3.down * GravityDownAcceleration * Time.deltaTime;
            }

            // Hit detection
            {
                RaycastHit closestHit = new RaycastHit();
                closestHit.distance = Mathf.Infinity;
                bool foundHit = false;

                // Sphere cast
                Vector3 displacementSinceLastFrame = Tip.position - m_LastRootPosition;
                RaycastHit[] hits = Physics.SphereCastAll(m_LastRootPosition, Radius,
                    displacementSinceLastFrame.normalized, displacementSinceLastFrame.magnitude, HittableLayers,
                    k_TriggerInteraction);
                foreach (var hit in hits)
                {
                    if (IsHitValid(hit) && hit.distance < closestHit.distance)
                    {
                        foundHit = true;
                        closestHit = hit;
                    }
                }

                if (foundHit)
                {
                    // Handle case of casting while already inside a collider
                    if (closestHit.distance <= 0f)
                    {
                        closestHit.point = Root.position;
                        closestHit.normal = -transform.forward;
                    }

                    OnHit(closestHit.point, closestHit.normal, closestHit.collider);
                }
            }

            m_LastRootPosition = Root.position;
        }

        bool IsHitValid(RaycastHit hit)
        {
            // ignore hits with an ignore component
            if (hit.collider.GetComponent<IgnoreHitDetection>())
            {
                return false;
            }

            // ignore hits with triggers that don't have a Damageable component
            if (hit.collider.isTrigger && hit.collider.GetComponent<Damageable>() == null)
            {
                return false;
            }

            // ignore hits with specific ignored colliders (self colliders, by default)
            if (m_IgnoredColliders != null && m_IgnoredColliders.Contains(hit.collider))
            {
                return false;
            }

            return true;
        }

        void OnHit(Vector3 point, Vector3 normal, Collider collider)
        {
            float damage = Damage * (m_ProjectileBase != null ? Mathf.Max(0.01f, m_ProjectileBase.DamageMultiplier) : 1f);

            // damage
            if (AreaOfDamage)
            {
                // area damage
                AreaOfDamage.InflictDamageInArea(damage, point, HittableLayers, k_TriggerInteraction,
                    m_ProjectileBase.Owner);
            }
            else
            {
                // point damage
                Damageable damageable = collider.GetComponent<Damageable>();
                if (damageable)
                {
                    damageable.InflictDamage(damage, false, m_ProjectileBase.Owner);
                }
            }

            // impact vfx
            if (ImpactVfx)
            {
                GameObject impactVfxInstance = Instantiate(ImpactVfx, point + (normal * ImpactVfxSpawnOffset),
                    Quaternion.LookRotation(normal));
                if (ImpactVfxLifetime > 0)
                {
                    Destroy(impactVfxInstance.gameObject, ImpactVfxLifetime);
                }
            }

            // impact sfx
            if (ImpactSfxClip)
            {
                AudioUtility.CreateSFX(ImpactSfxClip, point, AudioUtility.AudioGroups.Impact, 1f, 3f);
            }

            DetachTrail();

            // Self Destruct
            Destroy(this.gameObject);
        }

        void EnsureTrailRenderer()
        {
            if (!UseTravelTrail || m_TrailRenderer != null)
                return;

            GameObject trailObject = new GameObject("ProjectileTrail");
            trailObject.transform.SetParent(transform, false);
            trailObject.transform.localPosition = Vector3.zero;
            trailObject.transform.localRotation = Quaternion.identity;
            trailObject.transform.localScale = Vector3.one;

            m_TrailRenderer = trailObject.AddComponent<TrailRenderer>();
            m_TrailRenderer.enabled = true;
            m_TrailRenderer.time = TrailDuration;
            m_TrailRenderer.minVertexDistance = TrailMinVertexDistance;
            m_TrailRenderer.startWidth = TrailStartWidth;
            m_TrailRenderer.endWidth = TrailEndWidth;
            m_TrailRenderer.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
            m_TrailRenderer.receiveShadows = false;
            m_TrailRenderer.autodestruct = false;
            m_TrailRenderer.textureMode = LineTextureMode.Stretch;
            m_TrailRenderer.alignment = LineAlignment.View;
            m_TrailRenderer.startColor = TrailStartColor;
            m_TrailRenderer.endColor = TrailEndColor;

            if (TrailMaterial != null)
            {
                m_TrailRenderer.material = TrailMaterial;
            }
            else
            {
                Shader shader = Shader.Find("Sprites/Default");
                if (shader != null)
                {
                    Material material = new Material(shader);
                    material.color = TrailStartColor;
                    m_TrailRenderer.material = material;
                }
            }
        }

        void ResetTrail()
        {
            if (!UseTravelTrail)
                return;

            EnsureTrailRenderer();
            if (m_TrailRenderer == null)
                return;

            m_TrailDetached = false;
            m_TrailRenderer.transform.SetParent(transform, false);
            m_TrailRenderer.Clear();
            m_TrailRenderer.emitting = true;
        }

        void DetachTrail()
        {
            if (m_TrailDetached || m_TrailRenderer == null)
                return;

            m_TrailDetached = true;
            m_TrailRenderer.transform.SetParent(null, true);
            Destroy(m_TrailRenderer.gameObject, TrailDuration + 0.1f);
        }

        void OnDestroy()
        {
            DetachTrail();
        }

        void OnDrawGizmosSelected()
        {
            Gizmos.color = RadiusColor;
            Gizmos.DrawSphere(transform.position, Radius);
        }
    }
}