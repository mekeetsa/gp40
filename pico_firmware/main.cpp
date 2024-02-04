/* Galaksija USB Keyboard Interface
 *
 * The original Galaksija keyboard layout (similar to TRS-80 Model I):
 *
 *     .-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.
 *     |  !  |  "  |  #  |  $  |  %  |  &  |  '  |  (  |  )  |     |  *  |  -  |     |
 *     |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  0  |  :  |  =  | brk |
 * .---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.
 * |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
 * | up  |  Q  |  W  |  E  |  R  |  T  |  Y  |  U  |  I  |  O  |  P  |left |right| del |
 * '-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-----'-.---'-.
 *   |     |     |     |     |     |     |     |     |     |     |  +  |           |stop |
 *   |down |  A  |  S  |  D  |  F  |  G  |  H  |  J  |  K  |  L  |  ;  |    ret    |list |
 *   '-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.-----.---'-.---'
 *    |     |     |     |     |     |     |     |     |  <  |  >  |  ?  |     |     |
 *    |shift|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |rpt  |shift|
 *    '-----'-----'-----'-----'-----'-----'-----'-----'-----'-----'-----'-----'-----'
 *
 * The scan matrix (KC scan -> KR response):
 *
 *        KC=0       KC=1      KC=2      KC=3         KC=4      KC=5      KC=6
 * ----- ---------- --------- --------- ------------ --------- --------- -------------
 * KR=0  {0,1,NULL} {1,0,"H"} {2,0,"P"} {3,0,"X"   } {4,0,"0"} {5,0,"8"} {6,0,"ret"  }
 * KR=1  {0,1,"A" } {1,1,"I"} {2,1,"Q"} {3,1,"Y"   } {4,1,"1"} {5,1,"9"} {6,1,"brk"  }
 * KR=2  {0,2,"B" } {1,2,"J"} {2,2,"R"} {3,2,"Z"   } {4,2,"2"} {5,2,";"} {6,2,"rpt"  }
 * KR=3  {0,3,"C" } {1,3,"K"} {2,3,"S"} {3,3,"up"  } {4,3,"3"} {5,3,":"} {6,3,"del"  }
 * KR=4  {0,4,"D" } {1,4,"L"} {2,4,"T"} {3,4,"down"} {4,4,"4"} {5,4,","} {6,4,"list" }
 * KR=5  {0,4,"E" } {1,5,"M"} {2,5,"U"} {3,5,"left"} {4,5,"5"} {5,5,"="} {6,5,"shift"}
 * KR=6  {0,4,"F" } {1,6,"N"} {2,6,"V"} {3,6,"rght"} {4,6,"6"} {5,6,"."} {6,6,NULL   }
 * KR=7  {0,4,"G" } {1,7,"O"} {2,7,"w"} {3,7,"sp"  } {4,7,"7"} {5,7,"/"} {6,7,NULL   }
 *
 * Key bindings of the non-standard keys:
 *
 *  ESCAPE      -> brk
 *  DELETE      -> del
 *  INSERT      -> rpt
 *  APPLICATION -> list
 *  BACKSPACE   -> left
 *
 * The US keyboard layout:
 * .-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.-----.--------.
 * |  ~  |  !  |  @  |  #  |  $  |  %  |  ^  |  &  |  *  |  (  |  )  |  _  |  +  |        |
 * |  `  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  0  |  -  |  =  | BSP    |
 * +-----'-----'-----'-----'-----'-----'-----'-----'-----'-----'-----'-----'-----'--------+
 * |       |     |     |     |     |     |     |     |     |     |     |  {  |  }  |  |   |
 * | TAB   |  Q  |  W  |  E  |  R  |  T  |  Y  |  U  |  I  |  O  |  P  |  [  |  ]  |  \   |
 * +---.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'------+
 * | CAPS    |     |     |     |     |     |     |     |     |     |  :  |  "  |          |
 * | LOCK    |  A  |  S  |  D  |  F  |  G  |  H  |  J  |  K  |  L  |  ;  |  '  |   ENTER  |
 * +---'-----'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'-.---'----------+
 * |           |     |     |     |     |     |     |     |  <  |  >  |  ?  |              |
 * | SHIFT     |  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |    SHIFT     |
 * '-----------'-----'-----'-----'-----'-----'-----'-----'-----'-----'-----'--------------'
 */

#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"

#include "bsp/board.h"
#include "tusb.h"

#ifdef DEBUG
  #define debugf(...) printf(__VA_ARGS__)
#else
  #define debugf(...) 
#endif

///////////////////////////////////////////////////////////////////////////////
// KCKR hardware configuration

enum // GPIO0..GPIO15
{
  //
  // Column scan, inputs, pull-up enabled.
  // KC6-KC0 are connected to GPIO9-GPIO15.
  // Expected behavior:
  // - all KCs are pulled high by efault
  // - only one KC is pulled low during the keyboard scan
  //
  KC6 = 0,
  KC5 = 1,
  KC4 = 2,
  KC3 = 3,
  KC2 = 4,
  KC1 = 5,
  KC0 = 6,
  //
  // GPIO7 is an output that controls hardware reset of Galaksija.
  //
  GAL_RESET = 7,
  //
  // Row response, outputs.
  // KR7-KR0 are connected to GPIO0-GPIO7 (NOTE THE REVERSED BIT ORDER).
  //
  KR0 = 8,
  KR1 = 9,
  KR2 = 10,
  KR3 = 11,
  KR4 = 12,
  KR5 = 13,
  KR6 = 14,
  KR7 = 15
};

///////////////////////////////////////////////////////////////////////////////

// Map KC -> KR (note that KC is 7-bits wide)
volatile uint8_t scan_response[128] = { 0 };

static inline uint32_t get_KC ()
{
  // REVERSED ORDER: GPIO0 = KC6, GPIO1 = KC5, ...
  return gpio_get_all () & 0x7F;
}

static inline void put_KR( uint kc )
{
  // GPIO8 = KR0, GPIO9 = KR1, ...
  gpio_put_masked( /* mask */ 0xFF00, /* value */ scan_response[kc] << 8 );
}

// Keyboard scan response thread (running on the second core)
//
void KCKR_scan_response_thread ()
{
  for(;;)
  {
      put_KR( get_KC () );
  }
}

void KCKR_clear_scan_response ()
{
  for( uint i = 0; i < 128; ++i ) {
    scan_response[i] = 0x00;
  }
}

void KCKR_init ()
{
  KCKR_clear_scan_response ();

  // Set all output (GPIO7-GPIO15) to low
  //
  gpio_put_masked( /* mask */ 0xFF80, /* value */ 0x0000 );

  // Keyboard column scan
  //
  gpio_init( KC0 ); gpio_set_dir( KC0, GPIO_IN ); gpio_set_pulls( KC0, /*up*/true, /*down*/false );
  gpio_init( KC1 ); gpio_set_dir( KC1, GPIO_IN ); gpio_set_pulls( KC1, /*up*/true, /*down*/false );
  gpio_init( KC2 ); gpio_set_dir( KC2, GPIO_IN ); gpio_set_pulls( KC2, /*up*/true, /*down*/false );
  gpio_init( KC3 ); gpio_set_dir( KC3, GPIO_IN ); gpio_set_pulls( KC3, /*up*/true, /*down*/false );
  gpio_init( KC4 ); gpio_set_dir( KC4, GPIO_IN ); gpio_set_pulls( KC4, /*up*/true, /*down*/false );
  gpio_init( KC5 ); gpio_set_dir( KC5, GPIO_IN ); gpio_set_pulls( KC5, /*up*/true, /*down*/false );
  gpio_init( KC6 ); gpio_set_dir( KC6, GPIO_IN ); gpio_set_pulls( KC6, /*up*/true, /*down*/false );

  // Keyboard row response
  //
  gpio_init( KR0 ); gpio_set_dir( KR0, GPIO_OUT );
  gpio_init( KR1 ); gpio_set_dir( KR1, GPIO_OUT );
  gpio_init( KR2 ); gpio_set_dir( KR2, GPIO_OUT );
  gpio_init( KR3 ); gpio_set_dir( KR3, GPIO_OUT );
  gpio_init( KR4 ); gpio_set_dir( KR4, GPIO_OUT );
  gpio_init( KR5 ); gpio_set_dir( KR5, GPIO_OUT );
  gpio_init( KR6 ); gpio_set_dir( KR6, GPIO_OUT );
  gpio_init( KR7 ); gpio_set_dir( KR7, GPIO_OUT );

  // Galaksija reset
  //
  gpio_init( GAL_RESET ); gpio_set_dir( GAL_RESET, GPIO_OUT );

  // Set all output (GPIO8-GPIO0) to low
  //
  gpio_put_masked( /* mask */ 0xFF80, /* value */ 0x0000 );
}

///////////////////////////////////////////////////////////////////////////////
// Keyboard map

struct keycode_map {
  int kc;
  int kr;
  const char* name;
};

keycode_map keycode_to_KCKR[] =
{
//  KC  KR  NAME                       KEYCODE
// --- ---  ------- ----- --------------------
  { -1, -1, NULL    }, // (reserved)      0x00
  { -1, -1, NULL    }, // (reserved)      0x01
  { -1, -1, NULL    }, // (reserved)      0x02
  { -1, -1, NULL    }, // (reserved)      0x03
  {  0,  1, "A"     }, // A               0x04
  {  0,  2, "B"     }, // B               0x05
  {  0,  3, "C"     }, // C               0x06
  {  0,  4, "D"     }, // D               0x07
  {  0,  5, "E"     }, // E               0x08
  {  0,  6, "F"     }, // F               0x09
  {  0,  7, "G"     }, // G               0x0a
  {  1,  0, "H"     }, // H               0x0b
  {  1,  1, "I"     }, // I               0x0c
  {  1,  2, "J"     }, // J               0x0d
  {  1,  3, "K"     }, // K               0x0e
  {  1,  4, "L"     }, // L               0x0f
  {  1,  5, "M"     }, // M               0x10
  {  1,  6, "N"     }, // N               0x11
  {  1,  7, "O"     }, // O               0x12
  {  2,  0, "P"     }, // P               0x13
  {  2,  1, "Q"     }, // Q               0x14
  {  2,  2, "R"     }, // R               0x15
  {  2,  3, "S"     }, // S               0x16
  {  2,  4, "T"     }, // T               0x17
  {  2,  5, "U"     }, // U               0x18
  {  2,  6, "V"     }, // V               0x19
  {  2,  7, "W"     }, // W               0x1a
  {  3,  0, "X"     }, // X               0x1b
  {  3,  1, "Y"     }, // Y               0x1c
  {  3,  2, "Z"     }, // Z               0x1d
  {  4,  1, "1"     }, // 1               0x1e
  {  4,  2, "2"     }, // 2               0x1f
  {  4,  3, "3"     }, // 3               0x20
  {  4,  4, "4"     }, // 4               0x21
  {  4,  5, "5"     }, // 5               0x22
  {  4,  6, "6"     }, // 6               0x23
  {  4,  7, "7"     }, // 7               0x24
  {  5,  0, "8"     }, // 8               0x25
  {  5,  1, "9"     }, // 9               0x26
  {  4,  0, "0"     }, // 0               0x27
  {  6,  0, "ret"   }, // ENTER           0x28
  {  6,  1, "brk"   }, // ESCAPE          0x29
  {  3,  5, "left"  }, // BACKSPACE       0x2a
  {  3,  3, "up"    }, // TAB             0x2b
  {  3,  7, "sp"    }, // SPACE           0x2c
  { -1, -1, NULL    }, // MINUS           0x2d
  {  5,  5, "="     }, // EQUAL           0x2e
  {  3,  5, "left"  }, // BRACKET_LEFT    0x2f
  {  3,  6, "right" }, // BRACKET_RIGHT   0x30
  { -1, -1, NULL    }, // BACKSLASH       0x31
  { -1, -1, NULL    }, // EUROPE_1        0x32
  {  5,  2, ";"     }, // SEMICOLON       0x33
  {  5,  3, ":"     }, // APOSTROPHE      0x34
  { -1, -1, NULL    }, // GRAVE           0x35
  {  5,  4, ","     }, // COMMA           0x36
  {  5,  6, "."     }, // PERIOD          0x37
  {  5,  7, "/"     }, // SLASH           0x38
  {  3,  4, "down"  }, // CAPS_LOCK       0x39
  { -1, -1, NULL    }, // F1              0x3a
  { -1, -1, NULL    }, // F2              0x3b
  { -1, -1, NULL    }, // F3              0x3c
  { -1, -1, NULL    }, // F4              0x3d
  { -1, -1, NULL    }, // F5              0x3e
  { -1, -1, NULL    }, // F6              0x3f
  { -1, -1, NULL    }, // F7              0x40
  { -1, -1, NULL    }, // F8              0x41
  { -1, -1, NULL    }, // F9              0x42
  { -1, -1, NULL    }, // F10             0x43
  { -1, -1, NULL    }, // F11             0x44
  { -1, -1, NULL    }, // F12             0x45
  { -1, -1, NULL    }, // PRINT_SCREEN    0x46
  { -1, -1, NULL    }, // SCROLL_LOCK     0x47
  { -1, -1, NULL    }, // PAUSE           0x48
  { -1, -1, "rpt"   }, // INSERT          0x49
  { -1, -1, NULL    }, // HOME            0x4a
  { -1, -1, NULL    }, // PAGE_UP         0x4b
  {  6,  3, "del"   }, // DELETE          0x4c
  { -1, -1, NULL    }, // END             0x4d
  { -1, -1, NULL    }, // PAGE_DOWN       0x4e
  {  3,  6, "right" }, // ARROW_RIGHT     0x4f
  {  3,  5, "left"  }, // ARROW_LEFT      0x50
  {  3,  4, "down"  }, // ARROW_DOWN      0x51
  {  3,  3, "up"    }, // ARROW_UP        0x52
  { -1, -1, NULL    }, // NUM_LOCK        0x53
  { -1, -1, NULL    }, // KEYPAD_DIVIDE   0x54
  { -1, -1, NULL    }, // KEYPAD_MULTIPLY 0x55
  { -1, -1, NULL    }, // KEYPAD_SUBTRACT 0x56
  { -1, -1, NULL    }, // KEYPAD_ADD      0x57
  {  6,  0, "ret"   }, // KEYPAD_ENTER    0x58
  {  4,  1, "1"     }, // KEYPAD_1        0x59
  {  4,  2, "2"     }, // KEYPAD_2        0x5a
  {  4,  3, "3"     }, // KEYPAD_3        0x5b
  {  4,  4, "4"     }, // KEYPAD_4        0x5c
  {  4,  5, "5"     }, // KEYPAD_5        0x5d
  {  4,  6, "6"     }, // KEYPAD_6        0x5e
  {  4,  7, "7"     }, // KEYPAD_7        0x5f
  {  5,  0, "8"     }, // KEYPAD_8        0x60
  {  5,  1, "9"     }, // KEYPAD_9        0x61
  {  4,  0, "0"     }, // KEYPAD_0        0x62
  {  5,  6, "."     }, // KEYPAD_DECIMAL  0x63
  { -1, -1, NULL    }, // EUROPE_2        0x64
  {  6,  4, "list"  }, // APPLICATION     0x65
  { -1, -1, NULL    }, // POWER           0x66
  {  5,  5, "="     }, // KEYPAD_EQUAL    0x67
  // The regular keycodes stop at this point.
  // Below is an extension which incorporates SHIFT.
  {  6,  5, "SHIFT" }  // SHIFT           0x68
};

#define HID_KEY_MY_SHIFT 0x68

void KCKR_set( int keycode, bool on )
{
  debugf( "0x%02x %s", keycode, on ? "on " : "off" );

  if( keycode < 0 || keycode > HID_KEY_MY_SHIFT ) {
    debugf( "\n" );
    return;
  }

  const char* name = keycode_to_KCKR[keycode].name;
  int kc = 6 - keycode_to_KCKR[keycode].kc; // REVERSE ORDERED BITS
  int kr = keycode_to_KCKR[keycode].kr;

  if ( kc < 0 || kr < 0 || name == NULL ) {
    debugf( "\n" );
    return;
  }

  volatile uint8_t& state = scan_response[ ~( 1 << kc ) & 0x7F ];
  state = on ? state |  ( 1 << kr )
             : state & ~( 1 << kr );

  debugf( " -> %d, %d (%s)\n", 6 - kc, kr, name );
}

///////////////////////////////////////////////////////////////////////////////

struct Action {
  int keycode; // -1 = END, 0 = NOP (wait)
  bool on; // true = press, false = depress
  uint32_t interval_ms; // time to wait in this state
};

class MacroExecutor
{
  Action* action;
  int current = 0;
  uint32_t timeout_ms;

public:

  MacroExecutor( void )
  {
    action = NULL;
    current = -1;
    timeout_ms = 0;
  }

  bool is_idle( void )
  {
    return current < 0 || action == NULL || action[current].keycode < 0;
  }

  void start( Action* list )
  {
    action = list;
    current = 0;
    KCKR_set( action[current].keycode, action[current].on );
    timeout_ms = board_millis() + action[current].interval_ms;
  }

  void task()
  {
    if( is_idle() ) {
      return;
    }

    uint32_t current_ms = board_millis();
    if( current_ms >= timeout_ms )
    {
      // Current action expired. Proceed to the next step.
      ++current;
      if( action[current].keycode < 0 ) // End of macro
      {
        KCKR_clear_scan_response (); // clear all codes
        return;
      }

      KCKR_set( action[current].keycode, action[current].on );
      timeout_ms = current_ms + action[current].interval_ms;
    }
  }
};

///////////////////////////////////////////////////////////////////////////////

#define PRESS(key) \
  { key, true, 30 }, \
  { key, false, 30 }

#define PRESS_SHIFT(key) \
  { HID_KEY_MY_SHIFT, true, 0 }, \
  { key, true, 30 }, \
  { key, false, 30 }, \
  { HID_KEY_MY_SHIFT, false, 0 }

#define PRESS_END \
  { -1, false, 0 }

Action macro_USR_E000[] =
{
  PRESS( HID_KEY_A ),
  PRESS( HID_KEY_EQUAL ),
  PRESS( HID_KEY_U ),
  PRESS( HID_KEY_S ),
  PRESS( HID_KEY_R ),
  PRESS_SHIFT( HID_KEY_8 ), // "("
  PRESS_SHIFT( HID_KEY_6 ), // "&"
  PRESS( HID_KEY_E ),
  PRESS( HID_KEY_0 ),
  PRESS( HID_KEY_0 ),
  PRESS( HID_KEY_0 ),
  PRESS_SHIFT( HID_KEY_9 ), // ")"
  PRESS( HID_KEY_ENTER ),
  PRESS_END
};

Action macro_USR_F000[] =
{
  PRESS( HID_KEY_A ),
  PRESS( HID_KEY_EQUAL ),
  PRESS( HID_KEY_U ),
  PRESS( HID_KEY_S ),
  PRESS( HID_KEY_R ),
  PRESS_SHIFT( HID_KEY_8 ), // "("
  PRESS_SHIFT( HID_KEY_6 ), // "&"
  PRESS( HID_KEY_F ),
  PRESS( HID_KEY_0 ),
  PRESS( HID_KEY_0 ),
  PRESS( HID_KEY_0 ),
  PRESS_SHIFT( HID_KEY_9 ), // ")"
  PRESS( HID_KEY_ENTER ),
  PRESS_END
};

Action macro_DIR[] =
{
  PRESS( HID_KEY_D ),
  PRESS( HID_KEY_I ),
  PRESS( HID_KEY_R ),
  PRESS( HID_KEY_ENTER ),
  PRESS_END
};

Action macro_RUN[] =
{
  PRESS( HID_KEY_R ),
  PRESS( HID_KEY_U ),
  PRESS( HID_KEY_N ),
  PRESS( HID_KEY_ENTER ),
  PRESS_END
};

///////////////////////////////////////////////////////////////////////////////

MacroExecutor macroExecutor;

int main( void )
{
  // The KC/KR response time is below 200 ns at 125 MHz.
  // If the system clock speed is 240 MHz, it drops to 100 ns.
  // vreg_set_voltage( VREG_VOLTAGE_1_20 );
  // set_sys_clock_khz( 240000, true );

  board_init ();
  KCKR_init ();
  printf( "Galaksija Keyboard v0.5\n" );

  // Init host stack on configured roothub port
  tuh_init( BOARD_TUH_RHPORT );

  // Start the KC/KR scan response thread on core1
  multicore_launch_core1( KCKR_scan_response_thread );

  for( ;; )
  {
    tuh_task ();
    macroExecutor.task ();

    // Blink LED every interval_ms
    //
    static bool led_state = true;
    static uint32_t start_ms = 0;
    const uint32_t interval_ms1 = 100;
    const uint32_t interval_ms2 = 2000;
    if( led_state )
    {
      if( board_millis() >= start_ms + interval_ms2 )
      {
        start_ms += interval_ms2;
        board_led_write( led_state );
        led_state = ! led_state;
      }
    }
    else
    {
      if( board_millis() >= start_ms + interval_ms1 )
      {
        start_ms += interval_ms1;
        board_led_write( led_state );
        led_state = ! led_state;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// TinyUSB Callbacks

void tuh_mount_cb( uint8_t dev_addr ) // Application set-up
{
  printf( "Mounted %d\n", dev_addr );
}

void tuh_umount_cb( uint8_t dev_addr ) // Application tear-down
{
  printf( "Unmounted %d\n", dev_addr );
}

///////////////////////////////////////////////////////////////////////////////
// Each HID instance can has multiple reports

#define MAX_REPORT  4

static struct
{
  uint8_t report_count;
  tuh_hid_report_info_t report_info[MAX_REPORT];
} hid_info[CFG_TUH_HID];

static void process_kbd_report( hid_keyboard_report_t const *report);
static void process_mouse_report( hid_mouse_report_t const * report);
static void process_generic_report( uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len);

///////////////////////////////////////////////////////////////////////////////
// TinyUSB Callbacks
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// tuh_hid_mount_cb is invoked when device with hid interface is mounted.
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0.
//
void tuh_hid_mount_cb(
  uint8_t dev_addr, uint8_t instance,
  uint8_t const* desc_report, uint16_t desc_len
  )
{
  printf( "HID device address = %d, instance = %d is mounted\n", dev_addr, instance );

  // Interface protocol (hid_interface_protocol_enum_t)
  const char* protocol_str[] = { "None", "Keyboard", "Mouse" };
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  printf( "HID Interface Protocol = %s\n", protocol_str[itf_protocol] );

  // By default host stack will use activate boot protocol on supported interface.
  // Therefore for this simple example, we only need to parse generic report descriptor (with built-in parser)
  if ( itf_protocol == HID_ITF_PROTOCOL_NONE )
  {
    hid_info[instance].report_count = tuh_hid_parse_report_descriptor(hid_info[instance].report_info, MAX_REPORT, desc_report, desc_len);
    printf( "HID has %u reports\n", hid_info[instance].report_count );
  }

  // request to receive report
  // tuh_hid_report_received_cb() will be invoked when report is available
  if ( !tuh_hid_receive_report(dev_addr, instance) )
  {
    printf( "Error: cannot request to receive report\n" );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Invoked when device with hid interface is un-mounted
//
void tuh_hid_umount_cb( uint8_t dev_addr, uint8_t instance )
{
  printf( "HID device address = %d, instance = %d is unmounted\n", dev_addr, instance );
}

///////////////////////////////////////////////////////////////////////////////
// Invoked when received report from device via interrupt endpoint
//
void tuh_hid_report_received_cb(
  uint8_t dev_addr, uint8_t instance,
  uint8_t const* report, uint16_t len
  )
{
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  switch (itf_protocol)
  {
    case HID_ITF_PROTOCOL_KEYBOARD:
      TU_LOG2("HID receive boot keyboard report\n");
      process_kbd_report( (hid_keyboard_report_t const*) report );
    break;

    case HID_ITF_PROTOCOL_MOUSE:
      TU_LOG2("HID receive boot mouse report\n");
      process_mouse_report( (hid_mouse_report_t const*) report );
    break;

    default:
      // Generic report requires matching ReportID and contents with previous parsed report info
      process_generic_report(dev_addr, instance, report, len);
    break;
  }

  // continue to request to receive report
  if ( !tuh_hid_receive_report(dev_addr, instance) )
  {
    printf( "Error: cannot request to receive report\n" );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Look up new key in previous keys
//

// Previous report to check key released
//
static hid_keyboard_report_t prev_report = { 0, 0, {0} };

static inline bool find_key_in_report( hid_keyboard_report_t const* report, uint8_t keycode )
{
  for( int i = 0; i < 6; ++i )
  {
    if( report->keycode[i] == keycode ) {
      return true;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Keyboard Report
//
static void process_kbd_report( hid_keyboard_report_t const* report )
{
  //------------- example code ignore control (non-printable) key affects -------------//

  bool const is_prev_shift = prev_report.modifier
    & ( KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT );
  bool const is_this_shift = report->modifier
    & ( KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT );

  bool const is_ctrl_alt = ( ( report->modifier
    &  ( KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_LEFTALT ) )
    == ( KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_LEFTALT ) );
	  
  if ( is_prev_shift & ! is_this_shift )
  {
    KCKR_set( HID_KEY_MY_SHIFT, false ); // depress SHIFT
  }

  for( int i = 0; i < 6; ++i )
  {
    if( prev_report.keycode[i]
        && ! find_key_in_report( report, prev_report.keycode[i] )
      )
    {
      if( is_ctrl_alt && prev_report.keycode[i] == HID_KEY_DELETE )
      {
        debugf( "CTRL-ALT-DELETE off\n" );
	gpio_put( GAL_RESET, false );
      }
      else if( prev_report.keycode[i] >= HID_KEY_F1 && prev_report.keycode[i] <= HID_KEY_F12 )
      { 
        // macros (ignore)
      }
      else // depress the keycode that is missing from the previous report
      {
        KCKR_set( prev_report.keycode[i], false );
        board_led_write( false );
      }
    }
  }

  if ( ! is_prev_shift & is_this_shift )
  {
    KCKR_set( HID_KEY_MY_SHIFT, true ); // press SHIFT
  }

  for( int i = 0; i < 6; ++i )
  {
    if( report->keycode[i]
        && ! find_key_in_report( &prev_report, report->keycode[i] )
      )
    {
      if( is_ctrl_alt && report->keycode[i] == HID_KEY_DELETE )
      {
        debugf( "CTRL-ALT-DELETE on\n" );
	gpio_put( GAL_RESET, true );
      }
      else if( report->keycode[i] >= HID_KEY_F1 && report->keycode[i] <= HID_KEY_F12 )
      { 
        switch( report->keycode[i] )
	{
          case HID_KEY_F1: macroExecutor.start( macro_USR_E000 ); break;
          case HID_KEY_F2: macroExecutor.start( macro_USR_F000 ); break;
          case HID_KEY_F3: macroExecutor.start( macro_DIR ); break;
          case HID_KEY_F4: macroExecutor.start( macro_RUN ); break;
	}
      }
      else // press the newly arrived keycode
      {
        KCKR_set( report->keycode[i], true );
        board_led_write( true );
      }
    }
  }

  prev_report = *report;
}

///////////////////////////////////////////////////////////////////////////////
// Mouse Report
//
static void process_mouse_report( hid_mouse_report_t const* report )
{
  static hid_mouse_report_t prev_report = { 0 };

  //------------- button state  -------------//
  uint8_t button_changed_mask = report->buttons ^ prev_report.buttons;
  if ( button_changed_mask & report->buttons )
  {
    debugf(" %c%c%c ",
       report->buttons & MOUSE_BUTTON_LEFT   ? 'L' : '-',
       report->buttons & MOUSE_BUTTON_MIDDLE ? 'M' : '-',
       report->buttons & MOUSE_BUTTON_RIGHT  ? 'R' : '-');
  }

  //------------- cursor movement -------------//
  debugf("(%d %d %d)\n", report->x, report->y, report->wheel);
}

///////////////////////////////////////////////////////////////////////////////
// Generic Report
//
static void process_generic_report(
  uint8_t dev_addr, uint8_t instance,
  uint8_t const* report, uint16_t len
  )
{
  (void) dev_addr;

  uint8_t const rpt_count = hid_info[instance].report_count;
  tuh_hid_report_info_t* rpt_info_arr = hid_info[instance].report_info;
  tuh_hid_report_info_t* rpt_info = NULL;

  if ( rpt_count == 1 && rpt_info_arr[0].report_id == 0)
  {
    // Simple report without report ID as 1st byte
    rpt_info = &rpt_info_arr[0];
  }
  else
  {
    // Composite report, 1st byte is report ID, data starts from 2nd byte
    uint8_t const rpt_id = report[0];

    // Find report id in the array
    for(uint8_t i=0; i<rpt_count; i++)
    {
      if (rpt_id == rpt_info_arr[i].report_id )
      {
        rpt_info = &rpt_info_arr[i];
        break;
      }
    }

    ++report;
    --len;
  }

  if( ! rpt_info )
  {
    printf( "Couldn't find the report info for this report !\n" );
    return;
  }

  // For complete list of Usage Page & Usage checkout src/class/hid/hid.h. For examples:
  // - Keyboard                     : Desktop, Keyboard
  // - Mouse                        : Desktop, Mouse
  // - Gamepad                      : Desktop, Gamepad
  // - Consumer Control (Media Key) : Consumer, Consumer Control
  // - System Control (Power key)   : Desktop, System Control
  // - Generic (vendor)             : 0xFFxx, xx
  //
  if ( rpt_info->usage_page == HID_USAGE_PAGE_DESKTOP )
  {
    switch (rpt_info->usage)
    {
      case HID_USAGE_DESKTOP_KEYBOARD:
        // TU_LOG1("HID receive keyboard report\n");
        // Assume keyboard follow boot report layout
        process_kbd_report( (hid_keyboard_report_t const*) report );
      break;

      case HID_USAGE_DESKTOP_MOUSE:
        // TU_LOG1("HID receive mouse report\n");
        // Assume mouse follow boot report layout
        process_mouse_report( (hid_mouse_report_t const*) report );
      break;

      default: break;
    }
  }
}

