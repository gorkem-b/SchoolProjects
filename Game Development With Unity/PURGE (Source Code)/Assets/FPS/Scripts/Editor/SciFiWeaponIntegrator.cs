using System;
using System.Collections.Generic;
using System.IO;
using Unity.FPS.Game;
using UnityEditor;
using UnityEngine;

namespace PURGE.EditorTools
{
    public static class SciFiWeaponIntegrator
    {
        const string RifleWeaponPrefabPath = "Assets/FPS/Prefabs/Weapons/Weapon_SciFiRifle.prefab";
        const string PistolWeaponPrefabPath = "Assets/FPS/Prefabs/Weapons/Weapon_SciFiPistol.prefab";
        const string RifleVisualSourcePath = "Assets/Sci-Fi Rifle/Sci-Fi Rifle_fbx.FBX";
        const string PistolVisualSourcePath = "Assets/Sci-FiPistol01/Prefabs/Sci-Fi Pistol_Prefab.prefab";
        const string PlayerPrefabPath = "Assets/FPS/Prefabs/Player.prefab";

        [MenuItem("PURGE/Integrate Sci-Fi Starter Weapons")]
        public static void IntegrateSciFiStarterWeapons()
        {
            ConfigureWeaponPrefab(
                RifleWeaponPrefabPath,
                "Sci-Fi Rifle",
                "SciFiRifleVisual",
                RifleVisualSourcePath,
                new Vector3(-90f, 0f, 0f),
                0.95f,
                new Vector3(0f, -0.02f, 0.22f),
                controller =>
                {
                    controller.ShootType = WeaponShootType.Automatic;
                    controller.DelayBetweenShots = 0.08f;
                    controller.BulletSpreadAngle = 0f;
                    controller.MaxAmmo = 48;
                    controller.AimOffset = new Vector3(0f, 0.03f, -0.015f);
                });

            ConfigureWeaponPrefab(
                PistolWeaponPrefabPath,
                "Sci-Fi Pistol",
                "SciFiPistolVisual",
                PistolVisualSourcePath,
                new Vector3(0f, 0f, 0f),
                0.45f,
                new Vector3(0.018f, -0.062f, 0.148f),
                controller =>
                {
                    controller.ShootType = WeaponShootType.Manual;
                    controller.DelayBetweenShots = 0.18f;
                    controller.BulletSpreadAngle = 1f;
                    controller.MaxAmmo = 24;
                    controller.AimOffset = new Vector3(0f, 0.03f, -0.01f);
                });

            EnsurePlayerHasStartingWeapon(RifleWeaponPrefabPath);
            EnsurePlayerHasStartingWeapon(PistolWeaponPrefabPath);

            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
            Debug.Log("[SciFiWeaponIntegrator] Sci-Fi starter weapons integrated successfully.");
        }

        static void ConfigureWeaponPrefab(
            string weaponPrefabPath,
            string weaponName,
            string visualInstanceName,
            string visualSourcePath,
            Vector3 visualEuler,
            float targetMaxDimension,
            Vector3 targetOffset,
            Action<WeaponController> tuneWeapon)
        {
            GameObject prefabRoot = PrefabUtility.LoadPrefabContents(weaponPrefabPath);
            if (prefabRoot == null)
                throw new InvalidOperationException($"Could not load prefab contents: {weaponPrefabPath}");

            try
            {
                GameObject visualAsset = AssetDatabase.LoadAssetAtPath<GameObject>(visualSourcePath);
                if (visualAsset == null)
                    throw new InvalidOperationException($"Could not load visual asset: {visualSourcePath}");

                prefabRoot.name = Path.GetFileNameWithoutExtension(weaponPrefabPath);

                Transform gunRoot = prefabRoot.transform.Find("GunRoot");
                if (gunRoot == null)
                    gunRoot = prefabRoot.transform;

                Transform legacyVisual = FindChildRecursive(prefabRoot.transform, visualInstanceName);
                if (legacyVisual != null)
                    UnityEngine.Object.DestroyImmediate(legacyVisual.gameObject);

                Transform defaultWeaponMesh = gunRoot.Find("WeaponMesh_Pistol");
                if (defaultWeaponMesh != null)
                    defaultWeaponMesh.gameObject.SetActive(false);

                GameObject visualInstance = PrefabUtility.InstantiatePrefab(visualAsset, prefabRoot.scene) as GameObject;
                if (visualInstance == null)
                    visualInstance = UnityEngine.Object.Instantiate(visualAsset);

                visualInstance.name = visualInstanceName;
                visualInstance.transform.SetParent(gunRoot, false);
                visualInstance.transform.localPosition = Vector3.zero;
                visualInstance.transform.localRotation = Quaternion.Euler(visualEuler);
                visualInstance.transform.localScale = Vector3.one;

                var animator = visualInstance.GetComponentInChildren<Animator>(true);
                if (animator != null)
                    UnityEngine.Object.DestroyImmediate(animator);

                ApplyUrpMaterialsToVisual(visualInstance);

                ScaleVisualToMaxDimension(visualInstance, targetMaxDimension);
                visualInstance.transform.localPosition = targetOffset;

                WeaponController controller = prefabRoot.GetComponent<WeaponController>();
                if (controller != null)
                {
                    controller.WeaponName = weaponName;
                    tuneWeapon?.Invoke(controller);
                }

                PrefabUtility.SaveAsPrefabAsset(prefabRoot, weaponPrefabPath);
            }
            finally
            {
                PrefabUtility.UnloadPrefabContents(prefabRoot);
            }
        }

        static void EnsurePlayerHasStartingWeapon(string weaponPrefabPath)
        {
            GameObject playerRoot = PrefabUtility.LoadPrefabContents(PlayerPrefabPath);
            if (playerRoot == null)
                throw new InvalidOperationException($"Could not load player prefab: {PlayerPrefabPath}");

            try
            {
                GameObject weaponPrefab = AssetDatabase.LoadAssetAtPath<GameObject>(weaponPrefabPath);
                if (weaponPrefab == null)
                    throw new InvalidOperationException($"Could not load weapon prefab: {weaponPrefabPath}");

                WeaponController weaponController = weaponPrefab.GetComponent<WeaponController>();
                if (weaponController == null)
                    throw new InvalidOperationException($"Weapon prefab has no WeaponController: {weaponPrefabPath}");

                MonoBehaviour[] monoBehaviours = playerRoot.GetComponents<MonoBehaviour>();
                foreach (var behaviour in monoBehaviours)
                {
                    if (behaviour == null)
                        continue;

                    SerializedObject so = new SerializedObject(behaviour);
                    SerializedProperty startingWeapons = so.FindProperty("StartingWeapons");
                    if (startingWeapons == null || !startingWeapons.isArray)
                        continue;

                    bool exists = false;
                    for (int i = 0; i < startingWeapons.arraySize; i++)
                    {
                        SerializedProperty element = startingWeapons.GetArrayElementAtIndex(i);
                        if (element.objectReferenceValue == weaponController)
                        {
                            exists = true;
                            break;
                        }
                    }

                    if (!exists)
                    {
                        startingWeapons.InsertArrayElementAtIndex(startingWeapons.arraySize);
                        startingWeapons.GetArrayElementAtIndex(startingWeapons.arraySize - 1).objectReferenceValue = weaponController;
                        so.ApplyModifiedPropertiesWithoutUndo();
                    }

                    PrefabUtility.SaveAsPrefabAsset(playerRoot, PlayerPrefabPath);
                    return;
                }

                throw new InvalidOperationException("Could not find StartingWeapons array on Player prefab.");
            }
            finally
            {
                PrefabUtility.UnloadPrefabContents(playerRoot);
            }
        }

        static void ScaleVisualToMaxDimension(GameObject visualInstance, float targetMaxDimension)
        {
            Renderer[] renderers = visualInstance.GetComponentsInChildren<Renderer>(true);
            if (renderers.Length == 0)
                return;

            Bounds bounds = renderers[0].bounds;
            for (int i = 1; i < renderers.Length; i++)
                bounds.Encapsulate(renderers[i].bounds);

            float maxDimension = Mathf.Max(bounds.size.x, bounds.size.y, bounds.size.z);
            if (maxDimension > 0.0001f)
            {
                float scaleFactor = targetMaxDimension / maxDimension;
                visualInstance.transform.localScale = Vector3.one * scaleFactor;
            }
        }

        static void ApplyUrpMaterialsToVisual(GameObject visualInstance)
        {
            var renderers = visualInstance.GetComponentsInChildren<Renderer>(true);
            foreach (var renderer in renderers)
            {
                if (renderer == null)
                    continue;

                Material[] sharedMaterials = renderer.sharedMaterials;
                bool changed = false;

                for (int i = 0; i < sharedMaterials.Length; i++)
                {
                    Material source = sharedMaterials[i];
                    Material converted = GetOrCreateUrpMaterial(source);
                    if (converted != null && converted != source)
                    {
                        sharedMaterials[i] = converted;
                        changed = true;
                    }
                }

                if (changed)
                    renderer.sharedMaterials = sharedMaterials;
            }
        }

        static Material GetOrCreateUrpMaterial(Material source)
        {
            if (source == null)
                return null;

            if (source.shader != null && source.shader.name == "Universal Render Pipeline/Lit")
                return source;

            string sourcePath = AssetDatabase.GetAssetPath(source);
            if (string.IsNullOrEmpty(sourcePath))
                return source;

            string directory = Path.GetDirectoryName(sourcePath)?.Replace('\\', '/');
            string fileName = Path.GetFileNameWithoutExtension(sourcePath);
            string urpPath = $"{directory}/{fileName}_URP.mat";

            Material existing = AssetDatabase.LoadAssetAtPath<Material>(urpPath);
            if (existing != null)
                return existing;

            Shader urpShader = Shader.Find("Universal Render Pipeline/Lit");
            if (urpShader == null)
                return source;

            Material converted = new Material(urpShader);
            converted.name = fileName + "_URP";

            if (source.HasProperty("_MainTex"))
            {
                Texture mainTex = source.GetTexture("_MainTex");
                if (mainTex != null)
                    converted.SetTexture("_BaseMap", mainTex);
            }

            if (source.HasProperty("_Color"))
                converted.SetColor("_BaseColor", source.GetColor("_Color"));

            if (source.HasProperty("_BumpMap"))
            {
                Texture bump = source.GetTexture("_BumpMap");
                if (bump != null)
                {
                    converted.SetTexture("_BumpMap", bump);
                    converted.EnableKeyword("_NORMALMAP");
                }
            }

            if (source.HasProperty("_EmissionMap"))
            {
                Texture emissionMap = source.GetTexture("_EmissionMap");
                if (emissionMap != null)
                {
                    converted.SetTexture("_EmissionMap", emissionMap);
                    converted.EnableKeyword("_EMISSION");
                }
            }

            if (source.HasProperty("_EmissionColor"))
                converted.SetColor("_EmissionColor", source.GetColor("_EmissionColor"));

            if (source.HasProperty("_MetallicGlossMap"))
            {
                Texture metallic = source.GetTexture("_MetallicGlossMap");
                if (metallic != null)
                    converted.SetTexture("_MetallicGlossMap", metallic);
            }

            if (source.HasProperty("_Glossiness"))
                converted.SetFloat("_Smoothness", source.GetFloat("_Glossiness"));

            if (source.HasProperty("_Metallic"))
                converted.SetFloat("_Metallic", source.GetFloat("_Metallic"));

            AssetDatabase.CreateAsset(converted, urpPath);
            AssetDatabase.ImportAsset(urpPath);
            return converted;
        }

        static Transform FindChildRecursive(Transform root, string childName)
        {
            if (root.name == childName)
                return root;

            for (int i = 0; i < root.childCount; i++)
            {
                Transform result = FindChildRecursive(root.GetChild(i), childName);
                if (result != null)
                    return result;
            }

            return null;
        }
    }
}