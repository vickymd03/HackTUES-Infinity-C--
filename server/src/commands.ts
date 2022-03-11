export enum Organ {
    WheelFrontLeft,
    WheelFrontRight,
    WheelBackLeft,
    WheelBackRight,

    WheelSteerFrontLeft,
    WheelSteerFrontRight,
    WheelSteerBackLeft,
    WheelSteerBackRight,

    SolarPanel,
    Lights,
    Lamp,
    Camera,

    HandMain,
    HandJoint1,
    HandJoint2,
    HandClaw,

    SolarPanelPower,
    PowerSupply,

    Pause,
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

const parseMoveCommandFromJson = (json): MoveCommand => {
    let ret: MoveCommand;
    switch (json.organ) {
        case 'WheelFrontLeft':  ret.organ = Organ.WheelFrontLeft;  break;
        case 'WheelFrontRight': ret.organ = Organ.WheelFrontRight; break;
        case 'WheelBackLeft':   ret.organ = Organ.WheelBackLeft;   break;
        case 'WheelBackRight':  ret.organ = Organ.WheelBackRight;  break;
        default: return null;
    }

    if (typeof(json?.speed) !== 'number') return null;
    if (typeof(json?.time)  !== 'number') return null;

    ret.speed = json.speed;
    ret.time  = json.time;

    return ret;
}

const parseRotateCommandFromJson = (json): RotateCommand => {
    let ret: RotateCommand;
    switch (json.organ) {
        case 'WheelSteerFrontLeft':  ret.organ = Organ.WheelSteerFrontLeft;  break;
        case 'WheelSteerFrontRight': ret.organ = Organ.WheelSteerFrontRight; break;
        case 'WheelSteerBackLeft':   ret.organ = Organ.WheelSteerBackLeft;   break;
        case 'WheelSteerBackRight':  ret.organ = Organ.WheelSteerBackRight;  break;
        case 'SolarPanel':           ret.organ = Organ.SolarPanel;           break;
        case 'Camera':               ret.organ = Organ.Camera;               break;
        case 'HandMain':             ret.organ = Organ.HandMain;             break;
        case 'HandJoint1':           ret.organ = Organ.HandJoint1;           break;
        case 'HandJoint2':           ret.organ = Organ.HandJoint2;           break;
        case 'HandClaw':             ret.organ = Organ.HandClaw;             break;
        default: return null;
    }

    if (typeof(json?.degrees) !== 'number') return null;
    ret.degrees = json.degrees;

    return ret;
}

const parseBoolCommandFromJson = (json): BoolCommand => {
    let ret: BoolCommand;

    switch (json.organ) {
        case 'Lights':          ret.organ = Organ.Lights;          break;
        case 'Lamp':            ret.organ = Organ.Lamp;            break;
        case 'SolarPanelPower': ret.organ = Organ.SolarPanelPower; break;
        default: return null;
    }

    if (typeof(json?.bool) !== 'boolean') return null;
    ret.bool = json.bool;

    return ret;
}

const parsePowerCommandFromJson = (json): PowerCommand => {
    let ret: PowerCommand;

    if (json.organ !== 'PowerSupply') return null;
    ret.organ = Organ.PowerSupply;

    switch(json.state) {
        case 'On':    ret.state = PowerState.On;    break;
        case 'Sleep': ret.state = PowerState.Sleep; break;
        case 'Off':   ret.state = PowerState.Off;   break;
        default: return null;
    }

    return ret;
}

const parseTimeCommandFromJson = (json): TimeCommand => {
    let ret: TimeCommand;

    if (json.organ !== 'Pause') return null;
    ret.organ = Organ.Pause;

    if (typeof(json?.time) !== 'number') return null;
    ret.time = json.time;

    return ret;
}

export const parseCommandFromJson = (json): Command => {
    switch (json.type) {
        case 'Move':   return parseMoveCommandFromJson(json);
        case 'Rotate': return parseRotateCommandFromJson(json);
        case 'Bool':   return parseBoolCommandFromJson(json);
        case 'Power':  return parsePowerCommandFromJson(json);
        case 'Time':   return parseTimeCommandFromJson(json);
        default: return null;
    }
}
