export enum Organ {
    WheelFrontLeft  = "WheelFrontLeft",
    WheelFrontRight = "WheelFrontRight",
    WheelBackLeft   = "WheelBackLeft",
    WheelBackRight  = "WheelBackRight",

    WheelSteerFrontLeft  = "WheelSteerFrontLeft",
    WheelSteerFrontRight = "WheelSteerFrontRight",
    WheelSteerBackLeft   = "WheelSteerBackLeft",
    WheelSteerBackRight  = "WheelSteerBackRight",

    SolarPanel = "SolarPanel",
    Lights     = "Lights",
    Lamp       = "Lamp",
    Camera     = "Camera",

    HandMain   = "HandMain",
    HandJoint1 = "HandJoint1",
    HandJoint2 = "HandJoint2",
    HandClaw   = "HandClaw",

    SolarPanelPower = "SolarPanelPower",
    PowerSupply     = "PowerSupply",

    Pause = "Pause",
};

export enum CommandType {
    Move,
    Rotate,
    Bool,
    Power,
    Time,
}

export enum PowerState {
    On,
    Sleep,
    Off,
}

interface MoveCommand {
    type: CommandType.Move;
    organ:
        | Organ.WheelFrontLeft
        | Organ.WheelFrontRight
        | Organ.WheelBackLeft
        | Organ.WheelBackRight;
    speed: number;
    time:  number;
};

interface RotateCommand {
    type: CommandType.Rotate;
    organ:
        | Organ.WheelSteerFrontLeft
        | Organ.WheelSteerFrontRight
        | Organ.WheelSteerBackLeft
        | Organ.WheelSteerBackRight
        | Organ.SolarPanel
        | Organ.Camera
        | Organ.HandMain
        | Organ.HandJoint1
        | Organ.HandJoint2
        | Organ.HandClaw;
    degrees: number;
};

interface BoolCommand {
    type: CommandType.Bool
    organ:
        | Organ.Lights
        | Organ.Lamp
        | Organ.SolarPanelPower;
    bool: boolean;
}

interface PowerCommand {
    type: CommandType.Power;
    organ: Organ.PowerSupply;
    state: PowerState;
}

interface TimeCommand {
    type: CommandType.Time;
    organ: Organ.Pause;
    time: number;
}

export type Command =
    | MoveCommand
    | RotateCommand
    | BoolCommand
    | PowerCommand
    | TimeCommand;


let commandQueue:Command[] = [];
let commitedCommandQueue:Command[] = [];

export const enqueueCommand = (cmd: Command) => {
    if (commitedCommandQueue.length > 0) throw new Error('Can only enqueue if the commited queue is empty.');

    commandQueue.push(cmd);
}

export const commitQueue = () => {
    if (commitedCommandQueue.length > 0) throw new Error('Can only commit queue if the commited queue is empty.');

    for (const cmd of commandQueue) {
        commitedCommandQueue.push(cmd);
    }

    commandQueue = [];
}

export const getCommandQueue = () => {
    return commandQueue;
}

export const getCommitedCommandQueue = () => {
    return commitedCommandQueue;
}

export const clearCommitedCommandQueue = () => {
    return commitedCommandQueue;
}

const parseMoveCommandFromJson = (json): MoveCommand => {
    let organ;
    let speed;
    let time;

    switch (json.organ) {
        case 'WheelFrontLeft':  organ = Organ.WheelFrontLeft;  break;
        case 'WheelFrontRight': organ = Organ.WheelFrontRight; break;
        case 'WheelBackLeft':   organ = Organ.WheelBackLeft;   break;
        case 'WheelBackRight':  organ = Organ.WheelBackRight;  break;
        default: return null;
    }

    if (typeof(json?.speed) !== 'number') return null;
    if (typeof(json?.time)  !== 'number') return null;

    speed = json.speed;
    time  = json.time;

    return {
        type:  CommandType.Move,
        organ: organ,
        speed: speed,
        time:  time,
    };
}

const parseRotateCommandFromJson = (json): RotateCommand => {
    let organ;
    let degrees;

    switch (json.organ) {
        case 'WheelSteerFrontLeft':  organ = Organ.WheelSteerFrontLeft;  break;
        case 'WheelSteerFrontRight': organ = Organ.WheelSteerFrontRight; break;
        case 'WheelSteerBackLeft':   organ = Organ.WheelSteerBackLeft;   break;
        case 'WheelSteerBackRight':  organ = Organ.WheelSteerBackRight;  break;
        case 'SolarPanel':           organ = Organ.SolarPanel;           break;
        case 'Camera':               organ = Organ.Camera;               break;
        case 'HandMain':             organ = Organ.HandMain;             break;
        case 'HandJoint1':           organ = Organ.HandJoint1;           break;
        case 'HandJoint2':           organ = Organ.HandJoint2;           break;
        case 'HandClaw':             organ = Organ.HandClaw;             break;
        default: return null;
    }

    if (typeof(json?.degrees) !== 'number') return null;
    degrees = json.degrees;

    return {
        type:    CommandType.Rotate,
        organ:   organ,
        degrees: degrees,
    };
}

const parseBoolCommandFromJson = (json): BoolCommand => {
    let organ;
    let bool;

    switch (json.organ) {
        case 'Lights':          organ = Organ.Lights;          break;
        case 'Lamp':            organ = Organ.Lamp;            break;
        case 'SolarPanelPower': organ = Organ.SolarPanelPower; break;
        default: return null;
    }

    if (typeof(json?.bool) !== 'boolean') return null;
    bool = json.bool;

    return {
        type:  CommandType.Bool,
        organ: organ,
        bool: bool,
    };
}

const parsePowerCommandFromJson = (json): PowerCommand => {
    let organ;
    let state;

    if (json.organ !== 'PowerSupply') return null;
    organ = Organ.PowerSupply;

    switch(json.state) {
        case 'On':    state = PowerState.On;    break;
        case 'Sleep': state = PowerState.Sleep; break;
        case 'Off':   state = PowerState.Off;   break;
        default: return null;
    }

    return {
        type:  CommandType.Power,
        organ: organ,
        state: state,
    };
}

const parseTimeCommandFromJson = (json): TimeCommand => {
    let organ;
    let time;

    if (json.organ !== 'Pause') return null;
    organ = Organ.Pause;

    if (typeof(json?.time) !== 'number') return null;
    time = json.time;

    return {
        type:  CommandType.Time,
        organ: organ,
        time: time,
    };
}

export const parseCommandFromJson = (json): Command => {
    switch (json?.type) {
        case 'Move':   return parseMoveCommandFromJson(json);
        case 'Rotate': return parseRotateCommandFromJson(json);
        case 'Bool':   return parseBoolCommandFromJson(json);
        case 'Power':  return parsePowerCommandFromJson(json);
        case 'Time':   return parseTimeCommandFromJson(json);
        default: return null;
    }
}
