#pragma once

enum exceptions_x86
{
    ISR_DivisionError = 0,
    ISR_Debug,
    ISR_NonMaskableInterrupt,
    ISR_Breakpoint,
    ISR_Overflow,
    ISR_BoundRangeExceeded,
    ISR_InvalidOpcode,
    ISR_DeviceNotAvailable,
    ISR_DoubleFault,
    ISR_CoProcessorLegacy, //not used
    ISR_InvalidTSS,
    ISR_SegmentNotPresent,
    ISR_StackSegmentFault,
    ISR_GeneralProtectionFault, //err code
    ISR_PageFault, //err code
    //ISR_Reserved,
    ISR_x87FloatingPoint = 16,
    ISR_AlignmentCheck,
    ISR_MachineCheck,
    ISR_SIMDFloatingPoint,
    ISR_VirtualizationException,
    ISR_ControlProtectionException,
    //22 -27 reserved
    ISR_HypervisorInjection = 0x1c,
    ISR_VMMCommException,
    ISR_SecurityException,
};

const char *exception_names[] = {
    "ISR_DivisionError",
    "ISR_Debug",
    "ISR_NonMaskableInterrupt",
    "ISR_Breakpoint",
    "ISR_Overflow",
    "ISR_BoundRangeExceeded",
    "ISR_InvalidOpcode",
    "ISR_DeviceNotAvailable",
    "ISR_DoubleFault",
    "ISR_CoProcessorLegacy",
    "ISR_InvalidTSS",
    "ISR_SegmentNotPresent",
    "ISR_StackSegmentFault",
    "ISR_GeneralProtectionFault",
    "ISR_PageFault",
    "ISR_Reserved",
    "ISR_Reserved",
    "ISR_x87FloatingPoint",
    "ISR_AlignmentCheck",
    "ISR_MachineCheck",
    "ISR_SIMDFloatingPoint",
    "ISR_VirtualizationException",
    "ISR_ControlProtectionException",
    "ISR_Reserved",
    "ISR_Reserved",
    "ISR_Reserved",
    "ISR_Reserved",
    "ISR_Reserved",
    "ISR_HypervisorInjection",
    "ISR_VMMCommException",
    "ISR_SecurityException"
};