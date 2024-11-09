const REQUEST_5351_LOAD_SETTINGS = 0x10;
const REQUEST_5351_I2C_READ = 0x11;
const REQUEST_5351_I2C_WRITE = 0x12;

const REQUEST_COMMIT_SETTINGS = 0x20;
const REQUEST_RESET_SETTINGS = 0x21
const REQUEST_READ_SETTINGS = 0x22;
const REQUEST_WRITE_SETTINGS = 0x23;

const REQUEST_READ_SETTINGS_U32 = 0x30;
const REQUEST_WRITE_SETTINGS_U32 = 0x31;
const REQUEST_READ_SETTINGS_U16 = 0x32;
const REQUEST_WRITE_SETTINGS_U16 = 0x33;
const REQUEST_READ_SETTINGS_U8 = 0x34;
const REQUEST_WRITE_SETTINGS_U8 = 0x35;
const REQUEST_READ_SETTINGS_U64 = 0x36;
const REQUEST_WRITE_SETTINGS_U64 = 0x37;

const SettingsU8Offset = {
    SETTINGS_CLK0_SOURCE: 0,
    SETTINGS_CLK0_DRIVE: 1,
    SETTINGS_CLK0_PHASE: 2,
    SETTINGS_CLK0_INTEGER: 3,
    SETTINGS_CLK0_RDIV: 4,
    SETTINGS_CLK1_SOURCE: 5,
    SETTINGS_CLK1_DRIVE: 6,
    SETTINGS_CLK1_PHASE: 7,
    SETTINGS_CLK1_INTEGER: 8,
    SETTINGS_CLK1_RDIV: 9,
    SETTINGS_CLK2_SOURCE: 10,
    SETTINGS_CLK2_DRIVE: 11,
    SETTINGS_CLK2_PHASE: 12,
    SETTINGS_CLK2_INTEGER: 13,
    SETTINGS_CLK2_RDIV: 14,
    SETTINGS_SIZE_U8: 15,
};

const SettingsU16Offset = {
    SETTINGS_USB_VID: 0,
    SETTINGS_USB_PID: 1,
    SETTINGS_SIZE_U16: 2,
};

const SettingsU32Offset = {
    SETTINGS_PLL_A_MULT: 0,
    SETTINGS_PLL_A_NUM: 1,
    SETTINGS_PLL_A_DENOM: 2,
    SETTINGS_PLL_B_MULT: 3,
    SETTINGS_PLL_B_NUM: 4,
    SETTINGS_PLL_B_DENOM: 5,
    SETTINGS_CLK0_DIV: 6,
    SETTINGS_CLK0_NUM: 7,
    SETTINGS_CLK0_DENOM: 8,
    SETTINGS_CLK1_DIV: 9,
    SETTINGS_CLK1_NUM: 10,
    SETTINGS_CLK1_DENOM: 11,
    SETTINGS_CLK2_DIV: 12,
    SETTINGS_CLK2_NUM: 13,
    SETTINGS_CLK2_DENOM: 14,
    SETTINGS_SIZE_U32: 15,
};

export const Fxtal = 10000000;

export async function DeviceLoadSettings(device) {
    await device.controlTransferOut({
        requestType: 'class',
        recipient: 'interface',
        request: REQUEST_5351_LOAD_SETTINGS,
        value: 0,
        index: 0x05
    });
}

async function SettingsReadU8(device, offset) {
    let result = await device.controlTransferIn({
        requestType: 'class',
        recipient: 'interface',
        request: REQUEST_READ_SETTINGS_U8,
        value: offset,
        index: 0x05
    }, 1);
    return new Uint8Array(result.data.buffer)[0];
}

async function SettingsWriteU8(device, offset, value) {
    await device.controlTransferOut({
        requestType: 'class',
        recipient: 'interface',
        request: REQUEST_WRITE_SETTINGS_U8,
        value: offset,
        index: 0x05
    }, Uint8Array.from([value]).buffer);
}

async function SettingsReadU16(device, offset) {
    let result = await device.controlTransferIn({
        requestType: 'class',
        recipient: 'interface',
        request: REQUEST_READ_SETTINGS_U16,
        value: offset,
        index: 0x05
    }, 2);
    return new Uint16Array(result.data.buffer)[0];
}

async function SettingsWriteU16(device, offset, value) {
    await device.controlTransferOut({
        requestType: 'class',
        recipient: 'interface',
        request: REQUEST_WRITE_SETTINGS_U16,
        value: offset,
        index: 0x05
    }, Uint16Array.from([value]).buffer);
}

async function SettingsReadU32(device, offset) {
    let result = await device.controlTransferIn({
        requestType: 'class',
        recipient: 'interface',
        request: REQUEST_READ_SETTINGS_U32,
        value: offset,
        index: 0x05
    }, 4);
    return new Uint32Array(result.data.buffer)[0];
}

async function SettingsWriteU32(device, offset, value) {
    await device.controlTransferOut({
        requestType: 'class',
        recipient: 'interface',
        request: REQUEST_WRITE_SETTINGS_U32,
        value: offset,
        index: 0x05
    }, Uint32Array.from([value]).buffer);
}

export async function SettingsCommit(device) {
    await device.controlTransferOut({
        requestType: 'class',
        recipient: 'interface',
        request: REQUEST_COMMIT_SETTINGS,
        value: 0,
        index: 0x05
    });
}

export const Source = {
    SI5351_CLK_SRC_PLLA: 0,
    SI5351_CLK_SRC_PLLB: 1,
    SI5351_CLK_SRC_XTAL: 2,
}

const RDiv = {
    SI5351_R_DIV_1: 0,
    SI5351_R_DIV_2: 1,
    SI5351_R_DIV_4: 2,
    SI5351_R_DIV_8: 3,
    SI5351_R_DIV_16: 4,
    SI5351_R_DIV_32: 5,
    SI5351_R_DIV_64: 6,
    SI5351_R_DIV_128: 7,
}

function DivToRDiv(div) {
    switch (div) {
        case 1:
            return RDiv.SI5351_R_DIV_1;
        case 2:
            return RDiv.SI5351_R_DIV_2;
        case 4:
            return RDiv.SI5351_R_DIV_4;
        case 8:
            return RDiv.SI5351_R_DIV_8;
        case 16:
            return RDiv.SI5351_R_DIV_16;
        case 32:
            return RDiv.SI5351_R_DIV_32;
        case 64:
            return RDiv.SI5351_R_DIV_64;
        case 128:
            return RDiv.SI5351_R_DIV_128;
        default:
            return undefined;
    }
}

export const DriveStrength = {
    SI5351_DRIVE_STRENGTH_2MA: 0,
    SI5351_DRIVE_STRENGTH_4MA: 1,
    SI5351_DRIVE_STRENGTH_6MA: 2,
    SI5351_DRIVE_STRENGTH_8MA: 3,
    SI5351_DRIVE_STRENGTH_DISABLE: 4,
}

export class PLLConfig {
    constructor() {
        this.mult = 0;
        this.num = 0;
        this.denom = 0;
    }
}

export class OutputConfig {
    constructor() {
        this.allowIntegerMode = false;
        this.div = 0;
        this.num = 0;
        this.denom = 0;
        this.rdiv = 0;

        this.source = 0;
        this.drive = 0;
        this.phase = 0;
    }

    disable() {
        this.drive = DriveStrength.SI5351_DRIVE_STRENGTH_DISABLE;
    }
}

function gcd(a, b) {
    return b ? gcd(b, a % b) : a;
}

function approximateFraction(value, maxDenominator) {
    let a = Math.floor(value);
    let h1 = 1, k1 = 0, h = a, k = 1;
    let x = value - a;

    while (k <= maxDenominator) {
        if (x === 0) break;
        x = 1 / x;
        a = Math.floor(x);
        let h2 = h1, k2 = k1;
        h1 = h, k1 = k;
        h = a * h1 + h2;
        k = a * k1 + k2;
        if (h > maxDenominator || k > maxDenominator) return [h1, k1];
        if (Math.abs(x - a) < 1e-5) break;
        x = x - a;
    }

    return [h, k];
}

export function calculateConfig(outputClock) {
    let pll_conf = new PLLConfig();
    let out_conf = new OutputConfig();

    /*if (Fclk < 8000) Fclk = 8000;
    else if (Fclk > 160000000) Fclk = 160000000;*/
    if (outputClock < 1000000) {
        outputClock *= 64;
        out_conf.rdiv = RDiv.SI5351_R_DIV_64;
    } else {
        out_conf.rdiv = RDiv.SI5351_R_DIV_1;
    }

    out_conf.allowIntegerMode = true;

    let mult = outputClock / Fxtal;

    // Calculate best rational approximation of mult
    let [num, denom] = approximateFraction(mult, 1048575); // 1048575 is the max value for denom

    console.log(num, denom);
    
    let div = Math.round(750000000 / (num / denom * Fxtal));

    num *= div;

    let [a, b, c] = [Math.floor(num / denom), num % denom, denom];

    let [x, y, z] = [div, 0, 1];

    pll_conf.mult = a;
    pll_conf.num = b;
    pll_conf.denom = c;
    out_conf.div = x;
    out_conf.num = y;
    out_conf.denom = z;

    console.log(pll_conf, out_conf);

    return [pll_conf, out_conf];
}

export function calculateOutput(outputClock, pll_conf) {
    let out_conf = new OutputConfig();

    out_conf.allowIntegerMode = true;

    if (outputClock < 1000000) {
        outputClock *= 64;
        out_conf.rdiv = RDiv.SI5351_R_DIV_64;
    } else {
        out_conf.rdiv = RDiv.SI5351_R_DIV_1;
    }

    let Fpll = getPLLOutput(pll_conf);

    let mult = Fpll / outputClock;

    // Calculate best rational approximation of mult
    let [num, denom] = approximateFraction(mult, 1048575); // 1048575 is the max value for denom

    out_conf.div = Math.round(num / denom);
    out_conf.num = num % denom;
    out_conf.denom = denom;

    return out_conf;
}

export function calculateConfig2(outputClock) {
    let pll_conf = new PLLConfig();
    let out_conf = new OutputConfig();

    /*if (Fclk < 8000) Fclk = 8000;
    else if (Fclk > 160000000) Fclk = 160000000;*/

    out_conf.allowIntegerMode = true;

    if (outputClock < 1000000) {
        outputClock *= 64;
        out_conf.rdiv = RDiv.SI5351_R_DIV_64;
    } else {
        out_conf.rdiv = 0;
    }

    let a, b, c, x, y, z, t;

    if (outputClock < 81000000) {
        a = 90;
        b = 0;
        c = 1;
        let Fpll = 900000000;
        x = Math.floor(Fpll / outputClock);
        t = (outputClock >> 20) + 1;
        y = Math.floor((Fpll % outputClock) / t);
        z = Math.floor(outputClock / t);
    } else {
        if (outputClock >= 150000000) {
            x = 4;
        } else if (outputClock >= 100000000) {
            x = 6;
        } else {
            x = 8;
        }
        y = 0;
        z = 1;

        let numerator = x * outputClock;
        a = Math.floor(numerator / Fxtal);
        t = (Fxtal >> 20) + 1;
        b = Math.floor((numerator % Fxtal) / t);
        c = Math.floor(Fxtal / t);
    }

    pll_conf.mult = a;
    pll_conf.num = b;
    pll_conf.denom = c;
    out_conf.div = x;
    out_conf.num = y;
    out_conf.denom = z;

    console.log(pll_conf, out_conf);

    return [pll_conf, out_conf];
}

export function copyOutputDivider(dst, src) {
    dst.div = src.div;
    dst.num = src.num;
    dst.denom = src.denom;
    dst.rdiv = src.rdiv;
}

export async function setPLLConfig(device, pll, pll_conf) {
    if (pll === 0) {
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_PLL_A_MULT, pll_conf.mult);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_PLL_A_NUM, pll_conf.num);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_PLL_A_DENOM, pll_conf.denom);
    } else {
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_PLL_B_MULT, pll_conf.mult);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_PLL_B_NUM, pll_conf.num);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_PLL_B_DENOM, pll_conf.denom);
    }
}

export async function setOutputConfig(device, output, out_conf) {
    if (output === 0) {
        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK0_INTEGER, out_conf.allowIntegerMode ? 1 : 0);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_CLK0_DIV, out_conf.div);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_CLK0_NUM, out_conf.num);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_CLK0_DENOM, out_conf.denom);
        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK0_RDIV, out_conf.rdiv);

        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK0_SOURCE, out_conf.source);
        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK0_DRIVE, out_conf.drive);
        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK0_PHASE, out_conf.phase);
    } else if (output === 1) {
        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK1_INTEGER, out_conf.allowIntegerMode ? 1 : 0);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_CLK1_DIV, out_conf.div);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_CLK1_NUM, out_conf.num);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_CLK1_DENOM, out_conf.denom);
        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK1_RDIV, out_conf.rdiv);

        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK1_SOURCE, out_conf.source);
        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK1_DRIVE, out_conf.drive);
        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK1_PHASE, out_conf.phase);
    } else {
        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK2_INTEGER, out_conf.allowIntegerMode ? 1 : 0);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_CLK2_DIV, out_conf.div);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_CLK2_NUM, out_conf.num);
        await SettingsWriteU32(device, SettingsU32Offset.SETTINGS_CLK2_DENOM, out_conf.denom);
        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK2_RDIV, out_conf.rdiv);

        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK2_SOURCE, out_conf.source);
        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK2_DRIVE, out_conf.drive);
        await SettingsWriteU8(device, SettingsU8Offset.SETTINGS_CLK2_PHASE, out_conf.phase);
    }
}

export async function getPLLConfig(device, pll) {
    let pll_conf = new PLLConfig();
    if (pll === 0) {
        pll_conf.mult = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_PLL_A_MULT);
        pll_conf.num = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_PLL_A_NUM);
        pll_conf.denom = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_PLL_A_DENOM);
    } else {
        pll_conf.mult = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_PLL_B_MULT);
        pll_conf.num = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_PLL_B_NUM);
        pll_conf.denom = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_PLL_B_DENOM);
    }
    return pll_conf;
}


export async function getOutputConfig(device, output) {
    let out_conf = new OutputConfig();
    if (output === 0) {
        out_conf.allowIntegerMode = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK0_INTEGER) === 1;
        out_conf.div = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_CLK0_DIV);
        out_conf.num = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_CLK0_NUM);
        out_conf.denom = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_CLK0_DENOM);
        out_conf.rdiv = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK0_RDIV);

        out_conf.source = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK0_SOURCE);
        out_conf.drive = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK0_DRIVE);
        out_conf.phase = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK0_PHASE);
    } else if (output === 1) {
        out_conf.allowIntegerMode = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK1_INTEGER) === 1;
        out_conf.div = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_CLK1_DIV);
        out_conf.num = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_CLK1_NUM);
        out_conf.denom = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_CLK1_DENOM);
        out_conf.rdiv = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK1_RDIV);

        out_conf.source = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK1_SOURCE);
        out_conf.drive = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK1_DRIVE);
        out_conf.phase = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK1_PHASE);
    } else {
        out_conf.allowIntegerMode = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK2_INTEGER) === 1;
        out_conf.div = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_CLK2_DIV);
        out_conf.num = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_CLK2_NUM);
        out_conf.denom = await SettingsReadU32(device, SettingsU32Offset.SETTINGS_CLK2_DENOM);
        out_conf.rdiv = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK2_RDIV);

        out_conf.source = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK2_SOURCE);
        out_conf.drive = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK2_DRIVE);
        out_conf.phase = await SettingsReadU8(device, SettingsU8Offset.SETTINGS_CLK2_PHASE);
    }
    return out_conf;
}

export function getPLLOutput(pll_conf) {
    let Fpll = Fxtal * pll_conf.mult + Fxtal * pll_conf.num / pll_conf.denom;
    return Fpll;
}

export function getOutputClock(Fpll, out_conf) {
    let Fclk = Fpll / (out_conf.div + out_conf.num / out_conf.denom);
    Fclk /= 1 << out_conf.rdiv;
    return Fclk;
}