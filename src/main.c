#include <genesis.h>
#include <resources.h>

// BACKGROUNDS
int hscroll_offset_B = 0;
int hscroll_offset_A = 0;

// SPRITES
Sprite *player;
#define ANIM_STILL 0
#define ANIM_IDLE 1
#define ANIM_WALK 2
#define ANIM_UPPER 3

int attack_timer = 0;
int attack_duration = 56;

int player_x = 100;
int player_y = 50;
bool player_direction = FALSE;
int playerMovingSpeed = 2;

static void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);

    if (value & BUTTON_RIGHT)
    {
        SPR_setAnim(player, ANIM_WALK);
        player_direction = TRUE;
        SPR_setHFlip(player, player_direction);
        player_x += playerMovingSpeed;
    }
    else if (value & BUTTON_LEFT)
    {
        SPR_setAnim(player, ANIM_WALK);
        player_direction = FALSE;
        SPR_setHFlip(player, player_direction);
        player_x -= playerMovingSpeed;
    }
    else if (value & BUTTON_UP)
    {
        SPR_setAnim(player, ANIM_WALK);
        player_y -= playerMovingSpeed;
    }
    else if (value & BUTTON_DOWN)
    {
        SPR_setAnim(player, ANIM_WALK);
        player_y += playerMovingSpeed;
    }
    // else if (value & BUTTON_A)
    // {
    //     SPR_setAnim(player, ANIM_UPPER);
    // }
    else
    {
        SPR_setAnim(player, ANIM_IDLE);
    }

    SPR_setPosition(player, player_x, player_y);
}

static void joyEvent(u16 joy, u16 changed, u16 state)
{
    if ((changed & state & BUTTON_A) && attack_timer == 0)
    {
        SPR_setAnim(player, ANIM_UPPER);
        attack_timer += 1;
    }
}

static void attack()
{
    if (attack_timer == 0)
        handleInput();
    else if (attack_timer > 0 && attack_timer < attack_duration)
        attack_timer += 1;
    else
        attack_timer = 0;
}

void backgroundScroller(int A, int B)
{
    hscroll_offset_B -= A;
    hscroll_offset_A -= B;
    VDP_setHorizontalScroll(BG_B, hscroll_offset_B);
    VDP_setHorizontalScroll(BG_A, hscroll_offset_A);
}

int main()
{
    // Backgrounds
    u16 ind = TILE_USERINDEX;
    PAL_setPalette(PAL0, bg1.palette->data, DMA);
    VDP_drawImageEx(BG_B, &bg1, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind += bg1.tileset->numTile;
    PAL_setPalette(PAL1, fg1.palette->data, DMA);
    VDP_drawImageEx(BG_A, &fg1, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    VDP_setHorizontalScroll(BG_B, hscroll_offset_B);

    SPR_init(0, 0, 0);
    PAL_setPalette(PAL2, our_sprite.palette->data, DMA);
    player = SPR_addSprite(&our_sprite, 100, 50, TILE_ATTR(PAL2, FALSE, FALSE, TRUE));
    SPR_setAnim(player, ANIM_IDLE);

    JOY_setEventHandler(joyEvent);

    XGM_setLoopNumber(-1);
    XGM_startPlay(&track2);

    while (1)
    {
        backgroundScroller(1, 2);

        attack();

        SPR_update();
        SYS_doVBlankProcess();
    }
    return (0);
}
