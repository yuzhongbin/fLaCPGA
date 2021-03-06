transcript on
if {[file exists rtl_work]} {
    vdel -lib rtl_work -all
}
vlib rtl_work
vmap work rtl_work

vlog -vlog01compat -work work +incdir+/home/xavieran/BEngBSc/YEAR\ 5/FYP/fLaCPGA/Hardware {/home/xavieran/BEngBSc/YEAR 5/FYP/fLaCPGA/Hardware/SubframeDecoder.v}
vlog -vlog01compat -work work +incdir+/home/xavieran/BEngBSc/YEAR\ 5/FYP/fLaCPGA/Hardware {/home/xavieran/BEngBSc/YEAR 5/FYP/fLaCPGA/Hardware/RiceDecoder.v}
vlog -vlog01compat -work work +incdir+/home/xavieran/BEngBSc/YEAR\ 5/FYP/fLaCPGA/Hardware {/home/xavieran/BEngBSc/YEAR 5/FYP/fLaCPGA/Hardware/FixedDecoder.v}
vlog -vlog01compat -work work +incdir+/home/xavieran/BEngBSc/YEAR\ 5/FYP/fLaCPGA/Hardware {/home/xavieran/BEngBSc/YEAR 5/FYP/fLaCPGA/Hardware/RiceStreamReader.v}
vlog -vlog01compat -work work +incdir+/home/xavieran/BEngBSc/YEAR\ 5/FYP/fLaCPGA/Hardware {/home/xavieran/BEngBSc/YEAR 5/FYP/fLaCPGA/Hardware/ResidualDecoder.v}

vlog -vlog01compat -work work +incdir+/home/xavieran/BEngBSc/YEAR\ 5/FYP/fLaCPGA/Hardware {/home/xavieran/BEngBSc/YEAR 5/FYP/fLaCPGA/Hardware/SubframeDecoderTB.v}

vsim -t 1ps -L altera_ver -L lpm_ver -L sgate_ver -L altera_mf_ver -L altera_lnsim_ver -L cycloneii_ver -L rtl_work -L work -voptargs="+acc"  SubframeDecoderTB

onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -radix decimal /SubframeDecoderTB/file
add wave -noupdate -radix unsigned /SubframeDecoderTB/hi
add wave -noupdate -radix unsigned /SubframeDecoderTB/lo
add wave -noupdate -radix unsigned /SubframeDecoderTB/i
add wave -noupdate -radix unsigned /SubframeDecoderTB/clk
add wave -noupdate -radix unsigned /SubframeDecoderTB/rst
add wave -noupdate -radix unsigned /SubframeDecoderTB/ena
add wave -noupdate -radix unsigned /SubframeDecoderTB/wren
add wave -noupdate -radix unsigned /SubframeDecoderTB/frame_done
add wave -noupdate -radix unsigned /SubframeDecoderTB/rdaddr
add wave -noupdate -radix unsigned /SubframeDecoderTB/samples_read
add wave -noupdate -radix unsigned /SubframeDecoderTB/done
add wave -noupdate -radix decimal /SubframeDecoderTB/oData
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/block_size
add wave -noupdate -radix hexadecimal /SubframeDecoderTB/RamData
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/oReadAddr
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/rd/oReadAddr
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/rd_read_address
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/partition_order
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/predictor_order
add wave -noupdate -radix hexadecimal /SubframeDecoderTB/DUT/data_buffer
add wave -noupdate -radix hexadecimal /SubframeDecoderTB/DUT/rd/iData
add wave -noupdate -radix hexadecimal /SubframeDecoderTB/DUT/rd/data_buffer
add wave -noupdate -radix binary /SubframeDecoderTB/DUT/rd/data_buffer
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/rd_start_bit
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/rd/curr_bit
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/rd_done
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/rd_enable
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/rd_reset
add wave -noupdate -radix decimal /SubframeDecoderTB/DUT/rd_residual
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/rd/rs/oLSB
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/rd/rs/oMSB
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/rd/rs/oRiceParam
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/rd/rs/bits_remaining
add wave -noupdate /SubframeDecoderTB/DUT/rd/rs/iEnable
add wave -noupdate /SubframeDecoderTB/DUT/rd/rs/iData
add wave -noupdate /SubframeDecoderTB/DUT/rd/rs/oDone
add wave -noupdate /SubframeDecoderTB/DUT/rd/rd/oData
add wave -noupdate /SubframeDecoderTB/DUT/fd_enable
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/warmup_count
add wave -noupdate -radix decimal /SubframeDecoderTB/DUT/warmup_sample
add wave -noupdate -radix unsigned /SubframeDecoderTB/DUT/state
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {163883133 ps} 0}
quietly wave cursor active 1
configure wave -namecolwidth 420
configure wave -valuecolwidth 138
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ns
update
WaveRestoreZoom {163818734 ps} {164171854 ps}

view signals
run -all
