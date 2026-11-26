"""
SuperSideScroller 에셋/레벨 자동 생성 스크립트 (Unreal 10 ~ 13 PDF 실습 내용)

에디터에서 다시 실행하려면: 툴 > 파이썬 스크립트 실행 > 이 파일 선택
(이미 있는 에셋은 건너뛰므로 여러 번 실행해도 안전합니다.)
"""
import os
import traceback

import unreal

QUIT_WHEN_DONE = os.environ.get("SSS_QUIT_WHEN_DONE") == "1"
LOG_PATH = os.environ.get("SSS_LOG_PATH", "")

LOG = []


def log(msg):
    msg = str(msg)
    unreal.log("[SSS] " + msg)
    LOG.append(msg)


asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
EAL = unreal.EditorAssetLibrary
actor_subsys = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
level_subsys = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

# ---------------------------------------------------------------------------
# 경로
# ---------------------------------------------------------------------------
MAIN_ANIM = "/Game/MainCharacter/Animation"
MAIN_BP = "/Game/MainCharacter/Blueprints"
MAIN_PROJ = "/Game/MainCharacter/Projectile"
INPUT_DIR = "/Game/Input"
ENEMY_BP = "/Game/Enemy/Blueprints"
ENEMY_AI = "/Game/Enemy/AI"
ENEMY_MAT = "/Game/Enemy/Materials"
MAPS = "/Game/Maps"
LEVEL_PATH = MAPS + "/SuperSideScroller"

SKELETON = "/Game/Characters/Mannequins/Meshes/SK_Mannequin"
ABP_TEMPLATE = "/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed"
ANIM_IDLE = "/Game/Characters/Mannequins/Anims/Unarmed/MM_Idle"
ANIM_WALK = "/Game/Characters/Mannequins/Anims/Unarmed/Walk/MF_Unarmed_Walk_Fwd"
ANIM_RUN = "/Game/Characters/Mannequins/Anims/Unarmed/Jog/MF_Unarmed_Jog_Fwd"
ANIM_THROW = "/Game/Characters/Mannequins/Anims/Unarmed/Attack/MM_Attack_01"
ENEMY_MAT_PARENT = "/Game/Characters/Mannequins/Materials/Quinn/MI_Quinn_01"
GRID_MAT = "/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Gray"
GRID_MAT_DARK = "/Game/LevelPrototyping/Materials/MI_PrototypeGrid_TopDark"
CUBE_MESH = "/Engine/BasicShapes/Cube"
CYLINDER_MESH = "/Engine/BasicShapes/Cylinder"


def cpp_class(name):
    cls = unreal.load_class(None, "/Script/assignment_project." + name)
    if cls is None:
        raise RuntimeError("C++ class not found: " + name)
    return cls


def load(path):
    return EAL.load_asset(path) if EAL.does_asset_exist(path) else None


def create_asset(name, folder, asset_class, factory):
    path = folder + "/" + name
    if EAL.does_asset_exist(path):
        log("exists: " + path)
        return EAL.load_asset(path)
    asset = asset_tools.create_asset(name, folder, asset_class, factory)
    if asset is None:
        raise RuntimeError("create_asset failed: " + path)
    log("created: " + path)
    return asset


def save(asset):
    EAL.save_loaded_asset(asset, only_if_is_dirty=False)


def set_props(obj, **props):
    for key, value in props.items():
        obj.set_editor_property(key, value)


def make_key(name):
    key = unreal.Key()
    try:
        key.set_editor_property("key_name", name)
    except Exception:
        key.import_text(name)
    return key


def make_blueprint(name, folder, parent):
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent)
    bp = create_asset(name, folder, unreal.Blueprint, factory)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    return bp


def cdo_of(bp):
    return unreal.get_default_object(bp.generated_class())


STEPS = []


def step(fn):
    STEPS.append(fn)
    return fn


# ---------------------------------------------------------------------------
# 1. 폴더
# ---------------------------------------------------------------------------
@step
def make_folders():
    for d in [MAIN_ANIM, MAIN_BP, MAIN_PROJ, INPUT_DIR + "/Sprint", INPUT_DIR + "/Throw",
              ENEMY_BP, ENEMY_AI, ENEMY_MAT, MAPS]:
        EAL.make_directory(d)


# ---------------------------------------------------------------------------
# 2. 입력 (실습 11.04, 활동 11.03)
# ---------------------------------------------------------------------------
def input_action_factory():
    for name in ("InputAction_Factory", "InputActionFactory"):
        if hasattr(unreal, name):
            return getattr(unreal, name)()
    return None


def mapping_context_factory():
    for name in ("InputMappingContext_Factory", "InputMappingContextFactory"):
        if hasattr(unreal, name):
            return getattr(unreal, name)()
    return None


@step
def make_inputs():
    ia_sprint = create_asset("IA_Sprint", INPUT_DIR + "/Sprint", unreal.InputAction, input_action_factory())
    ia_sprint.set_editor_property("value_type", unreal.InputActionValueType.BOOLEAN)
    ia_sprint.set_editor_property("triggers", [unreal.new_object(unreal.InputTriggerDown, outer=ia_sprint)])
    save(ia_sprint)

    ia_throw = create_asset("IA_Throw", INPUT_DIR + "/Throw", unreal.InputAction, input_action_factory())
    ia_throw.set_editor_property("value_type", unreal.InputActionValueType.BOOLEAN)
    ia_throw.set_editor_property("triggers", [unreal.new_object(unreal.InputTriggerPressed, outer=ia_throw)])
    save(ia_throw)

    imc = create_asset("IC_SideScrollerCharacter", INPUT_DIR, unreal.InputMappingContext, mapping_context_factory())
    try:
        imc.unmap_all()
    except Exception:
        pass
    imc.map_key(ia_sprint, make_key("LeftShift"))
    imc.map_key(ia_sprint, make_key("Gamepad_LeftThumbstick"))
    imc.map_key(ia_throw, make_key("LeftMouseButton"))
    imc.map_key(ia_throw, make_key("Gamepad_RightTrigger"))
    save(imc)


# ---------------------------------------------------------------------------
# 3. 애니메이션 (실습 11.01, 12.01~12.03, 10.03 애님 블루프린트)
# ---------------------------------------------------------------------------
@step
def make_blend_space():
    skeleton = load(SKELETON)
    factory = unreal.BlendSpaceFactory1D()
    factory.set_editor_property("target_skeleton", skeleton)
    bs = create_asset("SideScroller_IdleRun_1D", MAIN_ANIM, unreal.BlendSpace1D, factory)

    try:
        params = list(bs.get_editor_property("blend_parameters"))
        p0 = params[0]
        p0.set_editor_property("display_name", "Speed")
        p0.set_editor_property("min", 0.0)
        p0.set_editor_property("max", 500.0)
        p0.set_editor_property("grid_num", 5)
        params[0] = p0
        bs.set_editor_property("blend_parameters", params)
    except Exception as e:
        log("blend_parameters not set: %s" % e)

    samples = []
    for anim_path, speed in ((ANIM_IDLE, 0.0), (ANIM_WALK, 300.0), (ANIM_RUN, 500.0)):
        s = unreal.BlendSample()
        s.set_editor_property("animation", load(anim_path))
        s.set_editor_property("sample_value", unreal.Vector(speed, 0.0, 0.0))
        try:
            s.set_editor_property("rate_scale", 1.0)
        except Exception:
            pass
        samples.append(s)
    bs.set_editor_property("sample_data", samples)
    save(bs)


@step
def make_montage():
    skeleton = load(SKELETON)
    factory = unreal.AnimMontageFactory()
    factory.set_editor_property("target_skeleton", skeleton)
    factory.set_editor_property("source_animation", load(ANIM_THROW))
    montage = create_asset("AM_Throw", MAIN_ANIM, unreal.AnimMontage, factory)
    save(montage)


@step
def make_anim_blueprints():
    template = load(ABP_TEMPLATE)
    for name, folder in (("AnimBP_SuperSideScroller_MainCharacter", MAIN_BP), ("AnimBP_Enemy", ENEMY_BP)):
        path = folder + "/" + name
        if EAL.does_asset_exist(path):
            log("exists: " + path)
            continue
        dup = asset_tools.duplicate_asset(name, folder, template)
        if dup is None:
            raise RuntimeError("duplicate failed: " + path)
        save(dup)
        log("duplicated ABP_Unarmed -> " + path)


# ---------------------------------------------------------------------------
# 4. 적 머티리얼 인스턴스 (실습 12.09)
# ---------------------------------------------------------------------------
@step
def make_enemy_material():
    factory = unreal.MaterialInstanceConstantFactoryNew()
    mi = create_asset("MI_Enemy01", ENEMY_MAT, unreal.MaterialInstanceConstant, factory)
    parent = load(ENEMY_MAT_PARENT)
    unreal.MaterialEditingLibrary.set_material_instance_parent(mi, parent)
    names = unreal.MaterialEditingLibrary.get_vector_parameter_names(mi)
    log("MI_Enemy01 vector params: %s" % [str(n) for n in names])
    red = unreal.LinearColor(1.0, 0.02, 0.02, 1.0)
    changed = 0
    for n in names:
        s = str(n).lower()
        if "color" in s or "tint" in s or "albedo" in s or "base" in s:
            unreal.MaterialEditingLibrary.set_material_instance_vector_parameter_value(mi, n, red)
            changed += 1
    log("MI_Enemy01 red params set: %d" % changed)
    unreal.MaterialEditingLibrary.update_material_instance(mi)
    save(mi)


# ---------------------------------------------------------------------------
# 5. AI 애셋 (실습 13.04, 13.05)
# ---------------------------------------------------------------------------
@step
def make_ai_assets():
    bb = create_asset("BB_EnemyAI", ENEMY_AI, unreal.BlackboardData, unreal.BlackboardDataFactory())
    keys = list(bb.get_editor_property("keys"))
    if not any(str(k.get_editor_property("entry_name")) == "MoveToLocation" for k in keys):
        entry = unreal.BlackboardEntry()
        entry.set_editor_property("entry_name", "MoveToLocation")
        vector_key_class = unreal.load_class(None, "/Script/AIModule.BlackboardKeyType_Vector")
        entry.set_editor_property("key_type", unreal.new_object(vector_key_class, outer=bb))
        keys.append(entry)
        bb.set_editor_property("keys", keys)
    save(bb)

    bt = create_asset("BT_EnemyAI", ENEMY_AI, unreal.BehaviorTree, unreal.BehaviorTreeFactory())
    bt.set_editor_property("blackboard_asset", bb)
    save(bt)


# ---------------------------------------------------------------------------
# 6. 블루프린트 (실습 10.03, 11.05, 12.07, 13.01, 13.07, 활동 13.03)
# ---------------------------------------------------------------------------
@step
def make_blueprints():
    # 발사체
    bp_proj = make_blueprint("BP_PlayerProjectile", MAIN_PROJ, cpp_class("PlayerProjectile"))
    save(bp_proj)

    # AI 컨트롤러
    bp_aic = make_blueprint("BP_AIControllerEnemy", ENEMY_AI, cpp_class("EnemyAIController"))
    bt = load(ENEMY_AI + "/BT_EnemyAI")
    save(bp_aic)

    # 정찰 지점
    bp_points = make_blueprint("BP_AIPoints", ENEMY_BP, cpp_class("AIPoints"))
    save(bp_points)

    # 플레이어 캐릭터
    bp_player = make_blueprint("BP_SuperSideScroller_MainCharacter", MAIN_BP, cpp_class("SuperSideScroller_Player"))
    cdo = cdo_of(bp_player)
    set_props(cdo,
              ic_character=load(INPUT_DIR + "/IC_SideScrollerCharacter"),
              ia_sprint=load(INPUT_DIR + "/Sprint/IA_Sprint"),
              ia_throw=load(INPUT_DIR + "/Throw/IA_Throw"),
              throw_montage=load(MAIN_ANIM + "/AM_Throw"),
              player_projectile=bp_proj.generated_class())
    abp_main = load(MAIN_BP + "/AnimBP_SuperSideScroller_MainCharacter")
    cdo.get_editor_property("mesh").set_editor_property("anim_class", abp_main.generated_class())
    save(bp_player)

    # 적
    bp_enemy = make_blueprint("BP_Enemy", ENEMY_BP, cpp_class("EnemyBase"))
    cdo = cdo_of(bp_enemy)
    cdo.set_editor_property("ai_controller_class", bp_aic.generated_class())
    mesh = cdo.get_editor_property("mesh")
    mesh.set_editor_property("anim_class", load(ENEMY_BP + "/AnimBP_Enemy").generated_class())
    mi = load(ENEMY_MAT + "/MI_Enemy01")
    if mi:
        mesh.set_material(0, mi)
    save(bp_enemy)

    # 게임 모드
    bp_gm = make_blueprint("BP_SuperSideScrollerGameMode", MAIN_BP, unreal.GameModeBase)
    cdo_of(bp_gm).set_editor_property("default_pawn_class", bp_player.generated_class())
    save(bp_gm)


# ---------------------------------------------------------------------------
# 7. 레벨 (활동 13.01, 실습 13.02, 13.07, 13.09)
# ---------------------------------------------------------------------------
def spawn(cls, loc, rot=None):
    rot = rot or unreal.Rotator(0, 0, 0)
    return actor_subsys.spawn_actor_from_class(cls, unreal.Vector(*loc), rot)


def Y(progress):
    # 화면 오른쪽 = 월드 -Y  (카메라 붐 Yaw 180)
    return -float(progress)


def block(label, p0, p1, top, thick=600.0, depth=400.0, mat=GRID_MAT, mesh=CUBE_MESH):
    length = float(p1 - p0)
    center = (Y((p0 + p1) / 2.0), 0.0, top - thick / 2.0)
    actor = actor_subsys.spawn_actor_from_object(load(mesh), unreal.Vector(0.0, center[0], center[2]))
    actor.set_actor_scale3d(unreal.Vector(depth / 100.0, length / 100.0, thick / 100.0))
    actor.set_actor_label(label)
    actor.set_folder_path("Level/Geometry")
    m = load(mat)
    if m:
        actor.static_mesh_component.set_material(0, m)
    return actor


@step
def make_level():
    if EAL.does_asset_exist(LEVEL_PATH):
        log("level exists, reusing (actors will be rebuilt)")
        level_subsys.load_level(LEVEL_PATH)
        for a in actor_subsys.get_all_level_actors():
            if a.get_folder_path() and str(a.get_folder_path()).startswith("Level"):
                actor_subsys.destroy_actor(a)
    else:
        level_subsys.new_level(LEVEL_PATH)

    # --- 조명 ---
    sun = spawn(unreal.DirectionalLight, (0, 0, 1000), unreal.Rotator(roll=0, pitch=-40, yaw=150))
    sun.set_actor_label("Sun")
    sun.set_folder_path("Level/Lighting")
    lc = sun.get_component_by_class(unreal.DirectionalLightComponent)
    try:
        lc.set_editor_property("atmosphere_sun_light", True)
    except Exception:
        pass
    lc.set_editor_property("intensity", 6.0)

    sky = spawn(unreal.SkyLight, (0, 0, 800))
    sky.set_folder_path("Level/Lighting")
    slc = sky.get_component_by_class(unreal.SkyLightComponent)
    try:
        slc.set_editor_property("real_time_capture", True)
    except Exception:
        pass
    atmo = spawn(unreal.SkyAtmosphere, (0, 0, 0))
    atmo.set_folder_path("Level/Lighting")
    fog = spawn(unreal.ExponentialHeightFog, (0, 0, -500))
    fog.set_folder_path("Level/Lighting")

    ppv = spawn(unreal.PostProcessVolume, (0, 0, 0))
    ppv.set_folder_path("Level/Lighting")
    ppv.set_editor_property("unbound", True)
    settings = ppv.get_editor_property("settings")
    settings.set_editor_property("override_auto_exposure_method", True)
    settings.set_editor_property("auto_exposure_method", unreal.AutoExposureMethod.AEM_HISTOGRAM)
    ppv.set_editor_property("settings", settings)

    # --- 지형 (진행 방향 = 화면 오른쪽) ---
    block("StartWall", -900, -800, 900)
    block("Ground_1", -800, 1800, 0)
    # 구덩이 1800 ~ 2100
    block("Ground_2", 2100, 3600, 0)
    block("Platform_A", 2450, 2800, 260, thick=60, mat=GRID_MAT_DARK)
    block("Platform_B", 2950, 3300, 480, thick=60, mat=GRID_MAT_DARK)
    block("Step_1", 3600, 3900, 150)
    block("Step_2", 3900, 4200, 300)
    # 구덩이 4200 ~ 4650 (떠 있는 발판으로 건너기)
    block("Platform_C", 4330, 4520, 330, thick=60, mat=GRID_MAT_DARK)
    block("Ground_3", 4650, 6800, 300)
    block("Platform_D", 5600, 5950, 600, thick=60, mat=GRID_MAT_DARK)
    block("EndWall", 6800, 6900, 1300)
    goal = block("Goal_Pillar", 6550, 6620, 700, thick=400, depth=70, mat=GRID_MAT_DARK, mesh=CYLINDER_MESH)
    goal.set_actor_label("Goal")

    # --- 플레이어 시작 지점 (화면 오른쪽을 바라봄) ---
    ps = spawn(unreal.PlayerStart, (0, Y(0), 110), unreal.Rotator(roll=0, pitch=0, yaw=-90))
    ps.set_folder_path("Level/Gameplay")

    # --- 내비 메시 바운드 볼륨 (실습 13.02 / 활동 13.01) ---
    nav = spawn(unreal.NavMeshBoundsVolume, (0, Y(3000), 300))
    nav.set_folder_path("Level/Gameplay")
    origin, extent = nav.get_actor_bounds(False)
    log("NavMeshBoundsVolume default extent: %s" % extent)
    if extent.x > 1.0:
        # 대략 X 1000, Y 8000, Z 2000 크기로 맞춤
        nav.set_actor_scale3d(unreal.Vector(500.0 / extent.x, 4200.0 / extent.y, 1000.0 / extent.z))
    origin, extent = nav.get_actor_bounds(False)
    log("NavMeshBoundsVolume extent after scale: %s" % extent)

    # RecastNavMesh-Default (내비 메시의 '두뇌') 가 없으면 생성 후 내비게이션 리빌드
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not unreal.GameplayStatics.get_all_actors_of_class(world, unreal.RecastNavMesh):
        recast = spawn(unreal.RecastNavMesh, (0, 0, 0))
        log("spawned RecastNavMesh: %s" % recast.get_name())
    unreal.SystemLibrary.execute_console_command(world, "RebuildNavigation")

    # --- 적 + 정찰 지점 (실습 13.07 ~ 13.09) ---
    bp_enemy = load(ENEMY_BP + "/BP_Enemy").generated_class()
    bp_points = load(ENEMY_BP + "/BP_AIPoints").generated_class()

    def enemy(label, progress, ground_top, half_range=None):
        loc = (0, Y(progress), ground_top + 100)
        e = spawn(bp_enemy, loc, unreal.Rotator(roll=0, pitch=0, yaw=90))
        e.set_actor_label(label)
        e.set_folder_path("Level/Enemies")
        if half_range:
            pts = spawn(bp_points, (0, Y(progress), ground_top + 5))
            pts.set_actor_label(label + "_PatrolPoints")
            pts.set_folder_path("Level/Enemies")
            pts.set_editor_property("points", [
                unreal.Vector(0, -half_range, 0),
                unreal.Vector(0, 0, 0),
                unreal.Vector(0, half_range, 0),
            ])
            e.set_editor_property("patrol_points", pts)
        return e

    enemy("Enemy_1", 1100, 0, 450)
    enemy("Enemy_2", 2900, 0, 500)
    enemy("Enemy_3", 5200, 300, 450)
    enemy("Enemy_4_RandomNav", 6100, 300)  # Patrol Points 없음 -> 내비 메시 위 임의 위치 (실습 13.05)

    # --- 월드 세팅: 게임 모드 / Kill Z ---
    world = unreal.EditorLevelLibrary.get_editor_world()
    ws = world.get_world_settings()
    gm = load(MAIN_BP + "/BP_SuperSideScrollerGameMode")
    ws.set_editor_property("default_game_mode", gm.generated_class())
    ws.set_editor_property("kill_z", -700.0)

    level_subsys.save_current_level()
    log("level saved: " + LEVEL_PATH)


def main():
    failed = []
    for fn in STEPS:
        try:
            log("== " + fn.__name__)
            fn()
        except Exception:
            failed.append(fn.__name__)
            log("FAILED " + fn.__name__ + "\n" + traceback.format_exc())
    try:
        EAL.save_directory("/Game/MainCharacter")
        EAL.save_directory("/Game/Enemy")
        EAL.save_directory("/Game/Input")
    except Exception:
        log("save_directory failed\n" + traceback.format_exc())
    log("DONE. failed steps: %s" % failed)

    if LOG_PATH:
        with open(LOG_PATH, "w", encoding="utf-8") as f:
            f.write("\n".join(LOG))
    if QUIT_WHEN_DONE:
        unreal.SystemLibrary.quit_editor()


main()
