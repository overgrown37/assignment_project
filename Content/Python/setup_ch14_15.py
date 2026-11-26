"""
SuperSideScroller 에셋/레벨 자동 생성 스크립트 (Unreal 14 ~ 15 PDF 실습 내용)

 14장: 애님 노티파이로 발사체 생성, 소켓, 적/발사체 VFX·SFX
 15장: 회전하는 수집 아이템(코인), 물약 파워업, 코인 UI, 플레이어 컨트롤러/게임 모드, 벽돌

에디터에서 다시 실행하려면: 툴 > 파이썬 스크립트 실행 > 이 파일 선택
(이미 있는 에셋은 건너뛰고 속성만 다시 지정하므로 여러 번 실행해도 안전합니다.)

교재에서 다른 샘플 프로젝트(Action RPG, Content Examples, Match 3 등)에서 이주하는 에셋은
엔진의 NetworkPredictionExtras 플러그인 에셋과 직접 만든 사운드(SourceAudio 폴더)로 대체했습니다.
"""
import os
import traceback

import unreal

QUIT_WHEN_DONE = os.environ.get("SSS_QUIT_WHEN_DONE") == "1"
LOG_PATH = os.environ.get("SSS_LOG_PATH", "")
LOG = []


def log(msg):
    msg = str(msg)
    unreal.log("[SSS15] " + msg)
    LOG.append(msg)


asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
EAL = unreal.EditorAssetLibrary
MEL = unreal.MaterialEditingLibrary
actor_subsys = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
level_subsys = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

PROJECT_DIR = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())

# ---------------------------------------------------------------------------
# 경로
# ---------------------------------------------------------------------------
MAIN_ANIM = "/Game/MainCharacter/Animation"
MAIN_BP = "/Game/MainCharacter/Blueprints"
MAIN_PROJ = "/Game/MainCharacter/Projectile"
MAIN_PC = "/Game/MainCharacter/PlayerController"
ENEMY_BP = "/Game/Enemy/Blueprints"
PICK = "/Game/PickableItems"
PICK_COL = PICK + "/Collectable"
PICK_POW = PICK + "/Powerup"
PICK_SND = PICK + "/Sounds"
PICK_MAT = PICK + "/Materials"
UI_DIR = "/Game/UI"
GM_DIR = "/Game/GameMode"
BRICK_DIR = "/Game/Brick"
LEVEL_PATH = "/Game/Maps/SuperSideScroller"

SKELETON = "/Game/Characters/Mannequins/Meshes/SK_Mannequin"
PLAYER_MESH = "/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple"

# 대체 에셋 (NetworkPredictionExtras 플러그인)
NPE = "/NetworkPredictionExtras"
FX_EXPLOSION = NPE + "/Art/Effects/Proto/Explosion/P_Explosion"            # P_Explosion (활동 14.02)
FX_ENEMY_DEATH = NPE + "/Art/Effects/Proto/Explosion/P_Grenade_Explosion"  # P_Goblin_Death 대체 (실습 14.09)
FX_PROJ_TRAIL = NPE + "/Art/Effects/Proto/Trails/P_Grenade_Trail"          # P_Env_Fire_Grate_01 대체 (실습 14.10)
FX_THROW_CAST = NPE + "/Art/Effects/Proto/Flares/P_Flare_Burst"            # P_Skill_001 대체 (실습 14.11)
FX_BRICK = NPE + "/Art/Effects/Proto/Sparks/P_Sparks"                      # P_Pixel_Explosion 대체 (실습 15.08)
SND_ENEMY_DEATH = NPE + "/Audio/Pain/HoffmanEfforts_PainMedium_01"         # A_Guardian_Death_Cue 대체
SND_PROJ_LOOP = NPE + "/Audio/SFX_Flame_01"                                # A_Ambient_Fire01_Cue 대체
SND_THROW_CAST = NPE + "/Audio/SciFiLaserShot_Cue"                         # A_Ability_FireballCast_Cue 대체
SND_EXPLOSION = NPE + "/Audio/MortarFirePlayer01"                          # Explosion_Cue 대체

CONE = "/Engine/BasicShapes/Cone"
CYLINDER = "/Engine/BasicShapes/Cylinder"
CUBE = "/Engine/BasicShapes/Cube"


def cpp_class(name):
    cls = unreal.load_class(None, "/Script/assignment_project." + name)
    if cls is None:
        raise RuntimeError("C++ class not found: " + name)
    return cls


def load(path):
    if EAL.does_asset_exist(path):
        return EAL.load_asset(path)
    log("MISSING asset: " + path)
    return None


def save(asset):
    EAL.save_loaded_asset(asset, only_if_is_dirty=False)


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


def make_blueprint(name, folder, parent):
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent)
    bp = create_asset(name, folder, unreal.Blueprint, factory)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    return bp


def cdo_of(bp):
    return unreal.get_default_object(bp.generated_class())


def try_set(obj, prop, value):
    try:
        obj.set_editor_property(prop, value)
        return True
    except Exception as e:
        log("could not set %s.%s: %s" % (obj.get_name(), prop, e))
        return False


STEPS = []


def step(fn):
    STEPS.append(fn)
    return fn


# ---------------------------------------------------------------------------
# 1. 폴더
# ---------------------------------------------------------------------------
@step
def make_folders():
    for d in [PICK, PICK_COL, PICK_POW, PICK_SND, PICK_MAT, UI_DIR, GM_DIR, BRICK_DIR, MAIN_PC]:
        EAL.make_directory(d)


# ---------------------------------------------------------------------------
# 2. 사운드 임포트 (Match_Combo / A_Character_Heal_Mana_Cue / Blueprints_TextPop01 대체)
# ---------------------------------------------------------------------------
@step
def import_sounds():
    src_dir = os.path.join(PROJECT_DIR, "SourceAudio")
    tasks = []
    for name in ("SW_CoinPickup", "SW_PowerupPickup", "SW_BrickPop"):
        if EAL.does_asset_exist(PICK_SND + "/" + name):
            log("exists: " + PICK_SND + "/" + name)
            continue
        t = unreal.AssetImportTask()
        t.set_editor_property("filename", os.path.join(src_dir, name + ".wav"))
        t.set_editor_property("destination_path", PICK_SND)
        t.set_editor_property("automated", True)
        t.set_editor_property("save", True)
        t.set_editor_property("replace_existing", True)
        tasks.append(t)
    if tasks:
        asset_tools.import_asset_tasks(tasks)
        for t in tasks:
            log("imported: %s" % list(t.get_editor_property("imported_object_paths")))


# ---------------------------------------------------------------------------
# 3. 머티리얼 (코인 / 물약 / 벽돌)
# ---------------------------------------------------------------------------
def make_color_material(name, color, metallic, roughness, emissive=0.0):
    path = PICK_MAT + "/" + name
    if EAL.does_asset_exist(path):
        log("exists: " + path)
        return EAL.load_asset(path)
    mat = create_asset(name, PICK_MAT, unreal.Material, unreal.MaterialFactoryNew())
    col = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector, -400, 0)
    col.set_editor_property("constant", unreal.LinearColor(color[0], color[1], color[2], 1.0))
    MEL.connect_material_property(col, "", unreal.MaterialProperty.MP_BASE_COLOR)
    met = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant, -400, 200)
    met.set_editor_property("r", metallic)
    MEL.connect_material_property(met, "", unreal.MaterialProperty.MP_METALLIC)
    rough = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant, -400, 300)
    rough.set_editor_property("r", roughness)
    MEL.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
    if emissive > 0.0:
        em = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector, -400, 400)
        em.set_editor_property("constant", unreal.LinearColor(color[0] * emissive, color[1] * emissive,
                                                              color[2] * emissive, 1.0))
        MEL.connect_material_property(em, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    MEL.recompile_material(mat)
    save(mat)
    return mat


@step
def make_materials():
    make_color_material("M_Coin", (1.0, 0.62, 0.08), 1.0, 0.25, 0.6)
    make_color_material("M_Potion", (0.85, 0.1, 1.0), 0.0, 0.15, 2.0)
    # M_Brick_Clay_Beveled 대체
    make_color_material("M_Brick", (0.55, 0.18, 0.07), 0.0, 0.85)


# ---------------------------------------------------------------------------
# 4. 소켓 (실습 14.04 ProjectileSocket, 실습 14.11 EffectSocket)
# ---------------------------------------------------------------------------
@step
def make_sockets():
    skm = load(PLAYER_MESH)
    skeleton = load(SKELETON)
    sockets = (
        # (이름, 본, 위치)  UE5 마네킹의 오른손 본은 hand_r
        ("ProjectileSocket", "hand_r", unreal.Vector(0.0, 0.0, 0.0)),
        # 교재는 Hips 본 기준 Y=100. UE5 마네킹은 root 본 축이 캐릭터 메시 축과 같으므로
        # root 기준으로 캐릭터 앞쪽(Y=100), 가슴 높이(Z=100) 에 둡니다.
        ("EffectSocket", "root", unreal.Vector(0.0, 100.0, 100.0)),
    )
    for name, bone, loc in sockets:
        ok = unreal.SSSEditorHelpers.add_skeleton_socket(skeleton, name, bone, loc)
        log("socket %s -> %s : %s" % (name, bone, ok))
    save(skeleton)
    save(skm)


# ---------------------------------------------------------------------------
# 5. AM_Throw 노티파이 (실습 14.02, 14.11)
# ---------------------------------------------------------------------------
@step
def make_montage_notifies():
    montage = load(MAIN_ANIM + "/AM_Throw")
    AL = unreal.AnimationLibrary
    length = AL.get_sequence_length(montage)
    # 교재는 30프레임 기준 22프레임째. 여기서는 공격 동작에서 손이 앞으로 나오는 시점으로 지정
    t = max(0.15, min(0.45, length * 0.35))
    log("AM_Throw length=%.3f, notify time=%.3f" % (length, t))

    notify_cls = cpp_class("Anim_ProjectileNotify")
    existing = AL.get_animation_notify_events(montage)
    existing_classes = set()
    for ev in existing:
        n = ev.get_editor_property("notify")
        if n:
            existing_classes.add(n.get_class().get_name())
    log("existing notifies: %s" % sorted(existing_classes))

    track_names = [str(n) for n in AL.get_animation_notify_track_names(montage)]
    for tr in ("ProjectileNotify", "VFX", "SFX"):
        if tr not in track_names:
            AL.add_animation_notify_track(montage, tr)

    if "Anim_ProjectileNotify" not in existing_classes:
        AL.add_animation_notify_event(montage, "ProjectileNotify", t, notify_cls)
        log("added Anim_ProjectileNotify")

    if "AnimNotify_PlayParticleEffect" not in existing_classes:
        fx = AL.add_animation_notify_event(montage, "VFX", t, unreal.AnimNotify_PlayParticleEffect)
        try_set(fx, "ps_template", load(FX_THROW_CAST))
        try_set(fx, "socket_name", "EffectSocket")
        try_set(fx, "scale", unreal.Vector(0.5, 0.5, 0.5))
        log("added Play Particle Effect")

    if "AnimNotify_PlaySound" not in existing_classes:
        snd = AL.add_animation_notify_event(montage, "SFX", t, unreal.AnimNotify_PlaySound)
        try_set(snd, "sound", load(SND_THROW_CAST))
        log("added Play Sound")

    save(montage)


# ---------------------------------------------------------------------------
# 6. 14장 블루프린트 속성 (발사체 / 적)
# ---------------------------------------------------------------------------
@step
def setup_projectile_and_enemy():
    bp_proj = load(MAIN_PROJ + "/BP_PlayerProjectile")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp_proj)
    cdo = cdo_of(bp_proj)
    try_set(cdo, "destroy_effect", load(FX_EXPLOSION))
    try_set(cdo, "destroy_sound", load(SND_EXPLOSION))
    try:
        cdo.get_editor_property("projectile_effect").set_editor_property("template", load(FX_PROJ_TRAIL))
    except Exception as e:
        log("projectile_effect: %s" % e)
    try:
        cdo.get_editor_property("projectile_movement_sound").set_editor_property("sound", load(SND_PROJ_LOOP))
    except Exception as e:
        log("projectile_movement_sound: %s" % e)
    save(bp_proj)

    bp_enemy = load(ENEMY_BP + "/BP_Enemy")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp_enemy)
    cdo = cdo_of(bp_enemy)
    try_set(cdo, "death_effect", load(FX_ENEMY_DEATH))
    try_set(cdo, "death_sound", load(SND_ENEMY_DEATH))
    save(bp_enemy)

    bp_player = load(MAIN_BP + "/BP_SuperSideScroller_MainCharacter")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp_player)
    cdo = cdo_of(bp_player)
    try_set(cdo, "throw_montage", load(MAIN_ANIM + "/AM_Throw"))
    try_set(cdo, "player_projectile", bp_proj.generated_class())
    save(bp_player)


# ---------------------------------------------------------------------------
# 7. 15장 블루프린트
# ---------------------------------------------------------------------------
def setup_pickable(bp, mesh_path, material, scale, mesh_rot, rot_rate, sound):
    cdo = cdo_of(bp)
    mesh = cdo.get_editor_property("mesh_comp")
    mesh.set_editor_property("static_mesh", load(mesh_path))
    if material:
        mesh.set_material(0, material)
    mesh.set_editor_property("relative_scale3d", scale)
    mesh.set_editor_property("relative_rotation", mesh_rot)
    cdo.get_editor_property("rotation_comp").set_editor_property("rotation_rate", rot_rate)
    try_set(cdo, "pickup_sound", sound)
    save(bp)


@step
def make_pickables():
    coin_snd = load(PICK_SND + "/SW_CoinPickup")
    pow_snd = load(PICK_SND + "/SW_PowerupPickup")

    # 실습 15.01 / 활동 15.01: 원뿔, 각 축 초당 100도 회전
    bp_base = make_blueprint("BP_PickableActor_Base", PICK, cpp_class("PickableActor_Base"))
    setup_pickable(bp_base, CONE, None, unreal.Vector(0.5, 0.5, 0.5), unreal.Rotator(0, 0, 0),
                   unreal.Rotator(roll=100, pitch=100, yaw=100), coin_snd)

    # 활동 15.02: 코인 (Z 축 초당 90도 회전)
    bp_coin = make_blueprint("BP_Collectable", PICK_COL, cpp_class("PickableActor_Collectable"))
    setup_pickable(bp_coin, CYLINDER, load(PICK_MAT + "/M_Coin"), unreal.Vector(0.6, 0.6, 0.08),
                   unreal.Rotator(roll=0, pitch=90, yaw=0), unreal.Rotator(roll=0, pitch=0, yaw=90), coin_snd)

    # 활동 15.03: 물약 파워업 (Pitch 초당 60도, Yaw 초당 180도)
    bp_pow = make_blueprint("BP_Powerup", PICK_POW, cpp_class("PickableActor_Powerup"))
    setup_pickable(bp_pow, CYLINDER, load(PICK_MAT + "/M_Potion"), unreal.Vector(0.3, 0.3, 0.5),
                   unreal.Rotator(0, 0, 0), unreal.Rotator(roll=0, pitch=60, yaw=180), pow_snd)


@step
def make_brick():
    # 실습 15.07 / 15.08
    bp = make_blueprint("BP_Brick", BRICK_DIR, cpp_class("SuperSideScroller_Brick"))
    cdo = cdo_of(bp)
    mesh = cdo.get_editor_property("brick_mesh")
    mesh.set_editor_property("static_mesh", load(CUBE))
    mesh.set_material(0, load(PICK_MAT + "/M_Brick"))
    mesh.set_editor_property("relative_scale3d", unreal.Vector(0.75, 0.75, 0.75))
    try_set(cdo, "hit_sound", load(PICK_SND + "/SW_BrickPop"))
    try_set(cdo, "explosion", load(FX_BRICK))
    save(bp)


@step
def make_ui_controller_gamemode():
    # 실습 15.04: 코인 카운터 위젯 블루프린트
    path = UI_DIR + "/BP_UI_CoinCollection"
    if EAL.does_asset_exist(path):
        widget_bp = EAL.load_asset(path)
        log("exists: " + path)
    else:
        factory = unreal.WidgetBlueprintFactory()
        factory.set_editor_property("parent_class", cpp_class("CoinCollectionWidget"))
        widget_bp = asset_tools.create_asset("BP_UI_CoinCollection", UI_DIR, unreal.WidgetBlueprint, factory)
        log("created: " + path)
    unreal.BlueprintEditorLibrary.compile_blueprint(widget_bp)
    save(widget_bp)

    # 실습 15.05: 플레이어 컨트롤러
    bp_pc = make_blueprint("BP_SuperSideScroller_PC", MAIN_PC, cpp_class("SuperSideScroller_Controller"))
    try_set(cdo_of(bp_pc), "coin_collection_widget_class", widget_bp.generated_class())
    save(bp_pc)

    # 실습 15.05: 게임 모드
    bp_gm = make_blueprint("BP_SuperSideScroller_GameMode", GM_DIR, unreal.GameModeBase)
    gm = cdo_of(bp_gm)
    gm.set_editor_property("player_controller_class", bp_pc.generated_class())
    gm.set_editor_property("default_pawn_class", load(MAIN_BP + "/BP_SuperSideScroller_MainCharacter").generated_class())
    save(bp_gm)


# ---------------------------------------------------------------------------
# 8. 레벨 배치
# ---------------------------------------------------------------------------
FOLDER = "Ch15"


def Y(progress):
    # 화면 오른쪽 = 월드 -Y  (카메라 붐 Yaw 180)
    return -float(progress)


def place(cls, label, progress, z, sub):
    a = actor_subsys.spawn_actor_from_class(cls, unreal.Vector(0.0, Y(progress), z), unreal.Rotator(0, 0, 0))
    a.set_actor_label(label)
    a.set_folder_path(FOLDER + "/" + sub)
    return a


@step
def populate_level():
    level_subsys.load_level(LEVEL_PATH)
    for a in actor_subsys.get_all_level_actors():
        fp = str(a.get_folder_path() or "")
        if fp.startswith(FOLDER):
            actor_subsys.destroy_actor(a)

    base = load(PICK + "/BP_PickableActor_Base").generated_class()
    coin = load(PICK_COL + "/BP_Collectable").generated_class()
    powerup = load(PICK_POW + "/BP_Powerup").generated_class()
    brick = load(BRICK_DIR + "/BP_Brick").generated_class()

    # 실습 15.01 회전하는 원뿔
    place(base, "PickableActor_Base", 250, 90, "Pickables")

    # 코인 (지형 높이: Ground_1/2 = 0, Platform_A = 260, Platform_B = 480, Platform_C = 330, Ground_3 = 300, Platform_D = 600)
    coins = [(400, 80), (480, 80), (560, 80),
             (1950, 260),                       # 첫 구덩이 위 (점프 중 획득)
             (2550, 330), (2625, 330), (2700, 330),
             (3050, 550), (3200, 550),
             (4425, 400),
             (4850, 380), (4930, 380), (5010, 380),
             (5700, 670), (5850, 670)]
    for i, (p, z) in enumerate(coins):
        place(coin, "Coin_%02d" % (i + 1), p, z, "Coins")

    # 물약 파워업
    place(powerup, "Powerup_1", 1700, 90, "Powerups")
    place(powerup, "Powerup_2", 3125, 550, "Powerups")

    # 벽돌 (아래에서 점프해 머리로 치기). 지면 + 420 정도면 일반 점프로 닿습니다.
    bricks = [("Brick_1_Coin", 700, 420, True, 1),
              ("Brick_2_Empty", 775, 420, False, 1),
              ("Brick_3_Coin", 850, 420, True, 1),
              ("Brick_4_Empty", 5150, 720, False, 1),
              ("Brick_5_Coin5", 5225, 720, True, 5)]
    for label, p, z, has, value in bricks:
        b = place(brick, label, p, z, "Bricks")
        # bool 프로퍼티 bHasCollectable 은 파이썬에서 b 접두사가 빠진 이름으로 노출됩니다
        if not try_set(b, "has_collectable", has):
            try_set(b, "b_has_collectable", has)
        b.set_editor_property("collectable_value", value)

    # 월드 세팅: 새 게임 모드 (실습 15.05)
    world = unreal.EditorLevelLibrary.get_editor_world()
    ws = world.get_world_settings()
    ws.set_editor_property("default_game_mode", load(GM_DIR + "/BP_SuperSideScroller_GameMode").generated_class())

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
    for d in ("/Game/MainCharacter", "/Game/Enemy", PICK, UI_DIR, GM_DIR, BRICK_DIR, "/Game/Characters/Mannequins/Meshes"):
        try:
            EAL.save_directory(d)
        except Exception:
            log("save_directory failed: " + d)
    log("DONE. failed steps: %s" % failed)

    if LOG_PATH:
        with open(LOG_PATH, "w", encoding="utf-8") as f:
            f.write("\n".join(LOG))
    if QUIT_WHEN_DONE:
        unreal.SystemLibrary.quit_editor()


main()
