var Video = {
    canvas : document.createElement("canvas"),
    stream : function() {
        this.canvas.width = 500;
        this.canvas.height = 350;
    }
}

function set_visibility(id, state) {
    document.getElementById(id).style.visibility = state;
}

async function submitWheelMovement(wheelId, organ) {
    const div = document.getElementById(wheelId);

    const speed   = Number(div.querySelector('#speed')  .value);
    const time    = Number(div.querySelector('#time')   .value);
    const degrees = Number(div.querySelector('#degrees').value);

    if (degrees != 0) {
        await fetch('/webinterface/command', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                type: 'Rotate',
                organ: 'WheelSteer' + organ,
                degrees: degrees,
            })
        });
    }

    if (speed != 0 && time != 0) {
        await fetch('/webinterface/command', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                type: 'Move',
                organ: 'Wheel' + organ,
                time: time,
                speed: speed,
            })
        });
    }
}

async function commitQueue() {
    await fetch('/webinterface/commit', { method: 'POST' });
}

window.addEventListener('DOMContentLoaded', (event) => {
    document.getElementById('btn-flw').addEventListener('click', () => submitWheelMovement('flm', 'FrontLeft'));
    document.getElementById('btn-frw').addEventListener('click', () => submitWheelMovement('frm', 'FrontRight'));
    document.getElementById('btn-rlw').addEventListener('click', () => submitWheelMovement('rlm', 'BackLeft'));
    document.getElementById('btn-rrw').addEventListener('click', () => submitWheelMovement('rrm', 'BackRight'));
    document.getElementById('commit').addEventListener('click', () => commitQueue());
});
