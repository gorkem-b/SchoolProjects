using System;
using System.Collections.Generic;
using Unity.FPS.AI;
using Unity.FPS.Game;
using UnityEngine;

namespace PURGE.Gameplay
{
    [DisallowMultipleComponent]
    [RequireComponent(typeof(EnemyController))]
    public class HoverbotAttackTelegraph : MonoBehaviour
    {
        [Header("Timing")]
        [Tooltip("How long the hoverbot should telegraph before a shot is allowed")]
        public float TelegraphLeadTime = 0.32f;

        [Tooltip("Minimum interval between telegraph sounds")]
        public float TelegraphSfxMinInterval = 0.1f;

        [Header("Visuals")]
        [Tooltip("Extra emissive color used while charging a shot")]
        [ColorUsage(true, true)]
        public Color TelegraphEyeColor = new Color(10f, 2.4f, 1.2f, 1f);

        [Tooltip("How intense the eye glow gets near release")]
        public float TelegraphEyeIntensityMultiplier = 2.15f;

        [Tooltip("How fast the eye pulse oscillates during telegraph")]
        public float TelegraphPulseFrequency = 9f;

        [Header("Audio")]
        [Tooltip("Optional pre-shot cue. If empty, the hoverbot detection cue is reused")]
        public AudioClip TelegraphSfx;

        [Tooltip("World-space min distance for the telegraph cue")]
        public float TelegraphSfxMinDistance = 6f;

        EnemyController m_EnemyController;
        DetectionModule m_DetectionModule;
        EnemyMobile m_EnemyMobile;
        WeaponController m_Weapon;
        Renderer m_EyeRenderer;
        int m_EyeMaterialIndex = -1;
        MaterialPropertyBlock m_EyePropertyBlock;
        readonly List<ParticleSystem> m_TelegraphVfx = new List<ParticleSystem>();

        bool m_IsTelegraphing;
        float m_TelegraphReleaseTime;
        float m_LastTelegraphSfxTime = Mathf.NegativeInfinity;

        void Awake()
        {
            m_EnemyController = GetComponent<EnemyController>();
            m_DetectionModule = GetComponentInChildren<DetectionModule>(true);
            m_EnemyMobile = GetComponent<EnemyMobile>();
            m_Weapon = GetComponentInChildren<WeaponController>(true);
            EnsureEyePropertyBlock();

            CacheEyeRenderer();
            CacheTelegraphVfx();
        }

        void OnEnable()
        {
            if (m_Weapon != null)
            {
                m_Weapon.ShootPermissionQuery += HandleShootPermissionQuery;
                m_Weapon.OnShootProcessed += OnShootProcessed;
            }
        }

        void OnDisable()
        {
            if (m_Weapon != null)
            {
                m_Weapon.ShootPermissionQuery -= HandleShootPermissionQuery;
                m_Weapon.OnShootProcessed -= OnShootProcessed;
            }

            StopTelegraph();
        }

        void Update()
        {
            if (!m_IsTelegraphing)
                return;

            if (!HasValidAttackTarget())
            {
                StopTelegraph();
                return;
            }

            ApplyTelegraphEyeColor();
        }

        bool HandleShootPermissionQuery()
        {
            if (TelegraphLeadTime <= 0f)
                return true;

            if (!HasValidAttackTarget())
            {
                StopTelegraph();
                return true;
            }

            if (!m_IsTelegraphing)
            {
                StartTelegraph();
                return false;
            }

            return Time.time >= m_TelegraphReleaseTime;
        }

        void OnShootProcessed()
        {
            StopTelegraph();
        }

        bool HasValidAttackTarget()
        {
            if (m_DetectionModule == null || m_EnemyMobile == null)
                return false;

            return m_DetectionModule.KnownDetectedTarget != null
                && m_DetectionModule.IsTargetInAttackRange
                && m_EnemyMobile.AiState == EnemyMobile.AIState.Attack;
        }

        void StartTelegraph()
        {
            m_IsTelegraphing = true;
            m_TelegraphReleaseTime = Time.time + TelegraphLeadTime;

            for (int i = 0; i < m_TelegraphVfx.Count; i++)
            {
                if (m_TelegraphVfx[i] != null)
                    m_TelegraphVfx[i].Play(true);
            }

            AudioClip cue = TelegraphSfx != null
                ? TelegraphSfx
                : m_EnemyMobile != null ? m_EnemyMobile.OnDetectSfx : null;

            if (cue != null && Time.time >= m_LastTelegraphSfxTime + TelegraphSfxMinInterval)
            {
                AudioUtility.CreateSFX(cue, transform.position, AudioUtility.AudioGroups.EnemyAttack, 1f,
                    TelegraphSfxMinDistance);
                m_LastTelegraphSfxTime = Time.time;
            }

            ApplyTelegraphEyeColor();
        }

        void StopTelegraph()
        {
            if (!m_IsTelegraphing && m_EyeRenderer == null)
                return;

            m_IsTelegraphing = false;

            for (int i = 0; i < m_TelegraphVfx.Count; i++)
            {
                if (m_TelegraphVfx[i] != null)
                    m_TelegraphVfx[i].Stop(true, ParticleSystemStopBehavior.StopEmittingAndClear);
            }

            ApplyBaseEyeColor();
        }

        void CacheEyeRenderer()
        {
            if (m_EnemyController == null || m_EnemyController.EyeColorMaterial == null)
                return;

            Renderer[] renderers = GetComponentsInChildren<Renderer>(true);
            for (int i = 0; i < renderers.Length; i++)
            {
                Material[] sharedMaterials = renderers[i].sharedMaterials;
                for (int materialIndex = 0; materialIndex < sharedMaterials.Length; materialIndex++)
                {
                    if (sharedMaterials[materialIndex] == m_EnemyController.EyeColorMaterial)
                    {
                        m_EyeRenderer = renderers[i];
                        m_EyeMaterialIndex = materialIndex;
                        return;
                    }
                }
            }
        }

        void CacheTelegraphVfx()
        {
            ParticleSystem[] particleSystems = GetComponentsInChildren<ParticleSystem>(true);
            for (int i = 0; i < particleSystems.Length; i++)
            {
                if (particleSystems[i] == null)
                    continue;

                if (particleSystems[i].gameObject.name.IndexOf("VFX_Alert", StringComparison.OrdinalIgnoreCase) >= 0)
                    m_TelegraphVfx.Add(particleSystems[i]);
            }
        }

        void ApplyBaseEyeColor()
        {
            if (m_EyeRenderer == null || m_EyeMaterialIndex < 0 || m_EnemyController == null)
                return;

            EnsureEyePropertyBlock();
            if (m_EyePropertyBlock == null)
                return;

            Color baseColor = m_DetectionModule != null && m_DetectionModule.KnownDetectedTarget != null
                ? m_EnemyController.AttackEyeColor
                : m_EnemyController.DefaultEyeColor;

            m_EyePropertyBlock.SetColor("_EmissionColor", baseColor);
            m_EyeRenderer.SetPropertyBlock(m_EyePropertyBlock, m_EyeMaterialIndex);
        }

        void ApplyTelegraphEyeColor()
        {
            if (m_EyeRenderer == null || m_EyeMaterialIndex < 0 || m_EnemyController == null)
                return;

            EnsureEyePropertyBlock();
            if (m_EyePropertyBlock == null)
                return;

            float normalized = 1f - Mathf.Clamp01((m_TelegraphReleaseTime - Time.time) / Mathf.Max(0.001f, TelegraphLeadTime));
            float pulse = 0.5f + 0.5f * Mathf.Sin(Time.time * TelegraphPulseFrequency);
            float blend = Mathf.Clamp01(Mathf.Lerp(normalized, 1f, pulse * normalized));

            Color startColor = m_EnemyController.AttackEyeColor;
            Color color = Color.Lerp(startColor, TelegraphEyeColor, blend);
            color *= Mathf.Lerp(1f, TelegraphEyeIntensityMultiplier, blend);

            m_EyePropertyBlock.SetColor("_EmissionColor", color);
            m_EyeRenderer.SetPropertyBlock(m_EyePropertyBlock, m_EyeMaterialIndex);
        }

        void EnsureEyePropertyBlock()
        {
            if (m_EyePropertyBlock == null)
                m_EyePropertyBlock = new MaterialPropertyBlock();
        }
    }
}