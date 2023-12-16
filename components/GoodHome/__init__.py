import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from . import sensor, number, select, switch, binary_sensor, text_sensor
from esphome.const import (
    CONF_ID,
    CONF_HUMIDITY,
    CONF_TEMPERATURE,
    CONF_TARGET_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_TIMESTAMP,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_SWITCH,
    DEVICE_CLASS_POWER_FACTOR,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    UNIT_MINUTE,
    UNIT_SECOND,
    ENTITY_CATEGORY_CONFIG,
    ENTITY_CATEGORY_DIAGNOSTIC
)

from .const import (
    goodhome_ns,
    CONF_ANTIF_TEMPERATURE,
    CONF_ECO_TEMPERATURE,
    CONF_COMF_TEMPERATURE,
    CONF_OVERRIDE_TEMPERATURE,
    CONF_TARGET_TEMPERATURE,
    CONF_CTN_ADJUST,
    CONF_WINDOW_TIMEOUT,
    CONF_LANGUAGE,
    CONF_TARGET_MODE,
    CONF_OPEN_WINDOW,
    CONF_SELF_LEARNING,
    CONF_MANUAL_MODE,
    CONF_PRESENCE_SENSOR,
    CONF_LEARNING_IMPROVE,
    CONF_DAYLIGHT_SAVING,
    CONF_ROOM_TYPE,
    CONF_ROOM_NAME,
    CONF_HARDWARE_VERSION,
    CONF_FIRMWARE_VERSION,
    CONF_CODE_NAME,
    CONF_DUTY_CYCLE,
    CONF_LONG_ABSENCE_TIMEOUT,
    CONF_SHORT_ABSENCE_TIMEOUT,
    CONF_LOCAL_TIME,
    CONF_SELF_LEARNING_COUNT,
    CONF_FAULT_COUNTER,
    CONF_RESTART_COUNTER,
    UNIT_DAY
)

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["json", "sensor", "number", "select", "switch", "binary_sensor", "text_sensor"]
MULTI_CONF = True

GoodHome = goodhome_ns.class_("GoodHome", cg.PollingComponent, uart.UARTDevice)

CONF_GOODHOME_ID = "goodhome_id"
GOODHOME_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_GOODHOME_ID): cv.use_id(GoodHome)
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(GoodHome),

            # NUMBER
            # Température de hors-gel, utilisée pendant les périodes d’absence de longue durée
            cv.Optional(CONF_ANTIF_TEMPERATURE): number.number_schema(
                min_value=7,
                max_value=15,
                step=0.5,
                icon="mdi:snowflake-thermometer",
                unit_of_measurement=UNIT_CELSIUS,
                device_class=DEVICE_CLASS_TEMPERATURE,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            # Température d’économie, utilisée pendant les périodes d’économie d’énergie et pendant les périodes d’absence de courte durée
            cv.Optional(CONF_ECO_TEMPERATURE): number.number_schema(
                min_value=9,
                max_value=25,
                step=0.5,
                icon="mdi:sun-thermometer-outline",
                unit_of_measurement=UNIT_CELSIUS,
                device_class=DEVICE_CLASS_TEMPERATURE,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            # Température de confort, utilisée pendant les périodes de présence
            cv.Optional(CONF_COMF_TEMPERATURE): number.number_schema(
                min_value=15,
                max_value=25,
                step=0.5,
                icon="mdi:sun-thermometer",
                unit_of_measurement=UNIT_CELSIUS,
                device_class=DEVICE_CLASS_TEMPERATURE,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            # Température de consigne
            cv.Required(CONF_TARGET_TEMPERATURE): number.number_schema(
                min_value=7,
                max_value=25,
                step=0.5,
                icon="mdi:thermometer-check",
                unit_of_measurement=UNIT_CELSIUS,
                device_class=DEVICE_CLASS_TEMPERATURE,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            # Consigne de température override
            cv.Required(CONF_OVERRIDE_TEMPERATURE): number.number_schema(
                min_value=7,
                max_value=25,
                step=0.5,
                icon="mdi:thermostat",
                unit_of_measurement=UNIT_CELSIUS,
                device_class=DEVICE_CLASS_TEMPERATURE,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            # Offset pour la calibration de la sonde de température
            cv.Optional(CONF_CTN_ADJUST): number.number_schema(
                min_value=-5,
                max_value=5,
                step=0.1,
                icon="mdi:thermometer-plus",
                unit_of_measurement=UNIT_CELSIUS,
                device_class=DEVICE_CLASS_TEMPERATURE,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            # Période de suspension de chauffe lorsqu’une fenêtre de la pièce est détectée ouverte (en minutes)
            cv.Optional(CONF_WINDOW_TIMEOUT): number.number_schema(
                min_value=1,
                max_value=180,
                step=1,
                icon="mdi:timer-pause-outline",
                unit_of_measurement=UNIT_MINUTE,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            # Date et heure locale de fin de la période d'absence de longue durée (Epoch Timestamp)
            cv.Optional(CONF_LONG_ABSENCE_TIMEOUT): number.number_schema(
                min_value=0,
                max_value=4102441200,
                step=1,
                icon="mdi:wallet-travel",
                unit_of_measurement=UNIT_SECOND,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            # Date et heure locale de fin de la période d'absence de courte durée (Epoch Timestamp)
            cv.Optional(CONF_SHORT_ABSENCE_TIMEOUT): number.number_schema(
                min_value=0,
                max_value=4102441200,
                step=1,
                icon="mdi:clock-time-eleven-outline",
                unit_of_measurement=UNIT_SECOND,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),

            # SELECT
            # Langue de l'interface de l'ecran LCD
            cv.Optional(CONF_LANGUAGE): select.select_schema(
                icon="mdi:earth",
                entity_category=ENTITY_CATEGORY_CONFIG,
                options={0: "French", 1: "English"}
            ),
            # Mode de fonctionnement
            # 0: Provisoire, retour au mode par défault à la mise sous tension
            # 1: Mode manuel, température confort, targetTemp=comfTemp (ou targetTemp=réglage depuis lcd)
            # 2: Mode manuel, température eco, targetTemp=ecoTemp
            # 3: Mode manuel, température anti-gel, targetTemp=antifTemp
            # 4: Provisoire, override, targetTemp=overrideTemp
            # 5: Absence de longue durée, targetTemp=antifTemp, fin définie par holidayTimeout
            # 8: Mode manuel, override, targetTemp=overrideTemp

            # 9: Mode auto/forcage manuel, température confort, targetTemp=comfTemp, retour en mode auto à la prochaine période
            # 10: Mode auto/forcage manuel, température eco, targetTemp=ecoTemp, retour en mode auto à la prochaine période
            # 12: Absence de courte durée, targetTemp=ecoTemp, fin définie par overrideTime

            # 60: Mode auto, période de présence (confort), targetTemp=comfTemp
            # 61: Mode auto, période d'absence (éco), targetTemp=ecoTemp
            cv.Required(CONF_TARGET_MODE): select.select_schema(
                icon="mdi:target",
                entity_category=ENTITY_CATEGORY_CONFIG,
                options={
                    0: "Default",
                    1: "Manual Comfort",
                    2: "Manual Energy-saving",
                    3: "Manual Frost protection",
                    5: "Long absence",
                    8: "Override",
                    9: "Forced Comfort",
                    10: "Forced Energy-saving",
                    12: "Short absence",
                    60: "Auto Comfort",
                    61: "Auto Energy-saving"
                }
            ),

            # SWITCH
            # Activer ou désactiver la détection de fenêtres ouvertes
            # La période de suspension de chauffe est définie par "windowTimeOut", en minutes
            cv.Optional(CONF_OPEN_WINDOW): switch.switch_schema(
                icon="mdi:window-open",
                device_class=DEVICE_CLASS_SWITCH,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            # Activer ou désactiver le mode AUTO LEARNING
            cv.Optional(CONF_SELF_LEARNING): switch.switch_schema(
                icon="mdi:brain",
                device_class=DEVICE_CLASS_SWITCH,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            # Activer ou désactiver le mode manuel (pas de programmation)
            cv.Optional(CONF_MANUAL_MODE): switch.switch_schema(
                icon="mdi:clock-outline",
                device_class=DEVICE_CLASS_SWITCH,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),
            # Activer ou désactiver la détection de présence
            # Notez que la détection de présence est obligatoire pour la programmation avec l’auto learning
            cv.Optional(CONF_PRESENCE_SENSOR): switch.switch_schema(
                icon="mdi:human-greeting",
                device_class=DEVICE_CLASS_SWITCH,
                entity_category=ENTITY_CATEGORY_CONFIG
            ),

            # BINARY SENSOR
            # Fin de la période d’apprentissage du mode AUTO LEARNING
            cv.Optional(CONF_LEARNING_IMPROVE): binary_sensor.binary_sensor_schema(
                icon="mdi:checkbox-marked-circle",
             ),
            # Activation du changement d’heure automatique
            cv.Optional(CONF_DAYLIGHT_SAVING): binary_sensor.binary_sensor_schema(
                icon="mdi:sun-clock-outline",
            ),

            # TEXT SENSOR
            # Type de pièce
            cv.Optional(CONF_ROOM_TYPE): text_sensor.text_sensor_schema(),
            # Nom de la pièce associée
            cv.Optional(CONF_ROOM_NAME): text_sensor.text_sensor_schema(),
            # Numéro de version Hardware
            cv.Optional(CONF_HARDWARE_VERSION): text_sensor.text_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),
            # Numéro de version Firmware
            cv.Optional(CONF_FIRMWARE_VERSION): text_sensor.text_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),
            # Produit
            cv.Optional(CONF_CODE_NAME): text_sensor.text_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),

            # SENSOR
            # Temperature courante
            cv.Required(CONF_TEMPERATURE): sensor.sensor_schema(
                icon="mdi:home-thermometer-outline",
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            # Humidité courante
            cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(
                icon="mdi:water-percent",
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            # Rapport Cyclique de pilotage de la résistance de chauffage. 0: pas de chauffe
            cv.Optional(CONF_DUTY_CYCLE): sensor.sensor_schema(
                icon="mdi:heat-wave",
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_POWER_FACTOR,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            # Date et heure locale (Epoch Timestamp)
            cv.Optional(CONF_LOCAL_TIME): sensor.sensor_schema(
                icon="mdi:clock-time-eight-outline",
                unit_of_measurement=UNIT_SECOND,
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            # Nombre de jours d'apprentissage de la période d'apprentissage
            cv.Optional(CONF_SELF_LEARNING_COUNT): sensor.sensor_schema(
                icon="mdi:counter",
                unit_of_measurement=UNIT_DAY,
                accuracy_decimals=0,
            ),
            # Compteur d'erreurs système
            cv.Optional(CONF_FAULT_COUNTER): sensor.sensor_schema(
                icon="mdi:alert-circle-outline",
                accuracy_decimals=0,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),
            # Compteur de redemarrage
            cv.Optional(CONF_RESTART_COUNTER): sensor.sensor_schema(
                icon="mdi:restart-alert",
                accuracy_decimals=0,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC
            ),
        }
    )
    .extend(cv.polling_component_schema("10min"))
    .extend(uart.UART_DEVICE_SCHEMA)
)
FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "GoodHome", baud_rate=115200, require_tx=True, require_rx=True
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    # NUMBER
    for key, tag_name, multiplier in [
        (CONF_ANTIF_TEMPERATURE, "antifTemp", 0.1),
        (CONF_ECO_TEMPERATURE, "ecoTemp", 0.1),
        (CONF_COMF_TEMPERATURE, "comfTemp", 0.1),
        (CONF_TARGET_TEMPERATURE, "targetTemp", 0.1),
        (CONF_OVERRIDE_TEMPERATURE, "overrideTemp", 0.1),
        (CONF_CTN_ADJUST, "ctnAdjust", 0.1),
        (CONF_WINDOW_TIMEOUT, "windowTimeOut", 1),
        (CONF_LONG_ABSENCE_TIMEOUT, "holidayTimeout", 1),
        (CONF_SHORT_ABSENCE_TIMEOUT, "overrideTime", 1),
    ]:
        if key in config:
            sens = await number.new_number(config[key], tag_name, multiplier)
            cg.add(var.register_listener(sens))

    # SELECT
    for key, tag_name in [
        (CONF_LANGUAGE, "language"),
        (CONF_TARGET_MODE, "targetMode"),
    ]:
        if key in config:
            sens = await select.new_select(config[key], tag_name)
            cg.add(var.register_listener(sens))

    # SWITCH
    for key, tag_name in [
        (CONF_OPEN_WINDOW, "window"),
        (CONF_SELF_LEARNING, "selfLearning"),
        (CONF_MANUAL_MODE, "noprog"),
        (CONF_PRESENCE_SENSOR, "occupancyStatus"),
    ]:
        if key in config:
            sens = await switch.new_switch(config[key], tag_name)
            cg.add(var.register_listener(sens))

    # BINARY SENSOR
    for key, tag_name in [
        (CONF_LEARNING_IMPROVE, "selfLearningImprove"),
        (CONF_DAYLIGHT_SAVING, "daylightSaving"),
    ]:
        if key in config:
            sens = await binary_sensor.new_binary_sensor(config[key], tag_name)
            cg.add(var.register_listener(sens))

    # TEXT SENSOR
    for key, tag_name in [
        (CONF_ROOM_TYPE, "roomType"),
        (CONF_ROOM_NAME, "roomName"),
        (CONF_HARDWARE_VERSION, "HwVer"),
        (CONF_FIRMWARE_VERSION, "fwVer"),
        (CONF_CODE_NAME, "codeName"),
    ]:
        if key in config:
            sens = await text_sensor.new_text_sensor(config[key], tag_name)
            cg.add(var.register_listener(sens))

    # SENSOR
    for key, tag_name, multiplier in [
        (CONF_TEMPERATURE, "currentTemp", 0.1),
        (CONF_HUMIDITY, "humidity", 0.1),
        (CONF_DUTY_CYCLE, "dutyCycle", 1),
        (CONF_LOCAL_TIME, "localTime", 1),
        (CONF_SELF_LEARNING_COUNT, "selfLearningCountDay", 1),
        (CONF_FAULT_COUNTER, "faultSystem", 1),
        (CONF_RESTART_COUNTER, "nbRestarts", 1),
    ]:
        if key in config:
            sens = await sensor.new_sensor(config[key], tag_name, multiplier)
            cg.add(var.register_listener(sens))
