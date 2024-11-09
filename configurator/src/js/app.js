import * as si5351 from './si5351'

document.addEventListener('alpine:init', () => {
    Alpine.store('device', {
        vid: 'CAFE',
        pid: '4012',
        device: undefined,
        plls: [new si5351.PLLConfig(), new si5351.PLLConfig()],
        outputs: [new si5351.OutputConfig(), new si5351.OutputConfig(), new si5351.OutputConfig()],
        synced: true,
        calculated: ["0", "0", "0"],
        saving: false,

        fetchConfig: async function () {
            if (!this.synced && !window.confirm("This will overwrite the current configuration. Are you sure?")) {
                return;
            }
            let device = this.device;
            this.plls = [
                await si5351.getPLLConfig(device, 0),
                await si5351.getPLLConfig(device, 1),
            ]
            this.outputs = [
                await si5351.getOutputConfig(device, 0),
                await si5351.getOutputConfig(device, 1),
                await si5351.getOutputConfig(device, 2),
            ]
            this.synced = true;
        },
        saveConfig: async function () {
            let device = this.device;
            this.saving = true;
            await new Promise(resolve => setTimeout(resolve, 500));
            for (let i = 0; i < 2; i++) {
                await si5351.setPLLConfig(device, i, this.plls[i]);
            }
            for (let i = 0; i < 3; i++) {
                await si5351.setOutputConfig(device, i, this.outputs[i]);
            }
            await si5351.SettingsCommit(device);
            await si5351.DeviceLoadSettings(device);
            this.synced = true;
            this.saving = false;
        },
        calculateCLKs: function () {
            const pllA = si5351.getPLLOutput(this.plls[0]);
            const pllB = si5351.getPLLOutput(this.plls[1]);

            let getInputFreq = (output) => {
                switch (output.source) {
                    case si5351.Source.SI5351_CLK_SRC_PLLA:
                        return pllA;
                    case si5351.Source.SI5351_CLK_SRC_PLLB:
                        return pllB;
                    case si5351.Source.SI5351_CLK_SRC_XTAL:
                        return si5351.Fxtal;
                    default:
                        return 0;
                }
            }

            for (let i = 0; i < 3; i++) {
                let output = this.outputs[i];
                let freq = si5351.getOutputClock(getInputFreq(output), output);
                if (isNaN(freq)) {
                    // Disable output on invalid frequency
                    this.outputs[i].disable();
                    freq = 0;
                }
                this.calculated[i] = (freq / 1000000).toFixed(9);
            }
        },
        calculateSimple(clk, freq) {
            freq = parseFloat(freq) * 1000000;
            if (clk == 0) {
                let [pll, output] = si5351.calculateConfig(freq);
                this.plls[0] = pll;
                // Do not overwrite phase or drive strength
                si5351.copyOutputDivider(this.outputs[0], output);
                this.outputs[0].source = si5351.Source.SI5351_CLK_SRC_PLLA;
            } else if (clk == 1) {
                if (this.outputs[0].drive === si5351.DriveStrength.SI5351_DRIVE_STRENGTH_DISABLE) {
                    let [pll, output] = si5351.calculateConfig(freq);
                    this.plls[0] = pll;
                    // Do not overwrite phase or drive strength
                    si5351.copyOutputDivider(this.outputs[1], output);
                    this.outputs[1].source = si5351.Source.SI5351_CLK_SRC_PLLA;
                } else {
                    // Use existing PLL
                    let output = si5351.calculateOutput(freq, this.plls[0]);

                    si5351.copyOutputDivider(this.outputs[1], output);
                    this.outputs[1].source = si5351.Source.SI5351_CLK_SRC_PLLA;
                }
            } else if (clk == 2) {
                let [pll, output] = si5351.calculateConfig(freq);
                this.plls[1] = pll;
                // Do not overwrite phase or drive strength
                si5351.copyOutputDivider(this.outputs[2], output);
                this.outputs[2].source = si5351.Source.SI5351_CLK_SRC_PLLB;
            }
            this.synced = false;
            this.calculateCLKs();
        },
        async connect() {
            if (this.device) {
                await this.device.close();
                this.device = undefined;
                this.connected = false;
            } else {
                let vid = parseInt(this.vid, 16);
                let pid = parseInt(this.pid, 16);

                device = await navigator.usb.requestDevice({
                    filters: [{ vendorId: vid, productId: pid }]
                });
                await device.open();
                await device.selectConfiguration(1);
                await device.claimInterface(5);

                this.device = device;
                await this.fetchConfig();
                this.calculateCLKs();
                this.synced = true;
                this.connected = true;
            }
        }
    })
    navigator.usb.addEventListener('disconnect', (event) => {
        if (event.device === Alpine.store('device').device) {
            Alpine.store('device').device = undefined;
            Alpine.store('device').connected = false;
        }
    });
})
