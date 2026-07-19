rm -rf build_dir.hw_emu.xilinx_u55c_gen3x16_xdma_3_202210_1
rm -rf tmp_kernel_pack_merge_hw_emu_xilinx_u55c_gen3x16_xdma_3_202210_1
rm -rf packaged_kernel_merge_hw_emu_xilinx_u55c_gen3x16_xdma_3_202210_1
rm -rf _x.hw_emu.xilinx_u55c_gen3x16_xdma_3_202210_1
rm -rf build_dir.hw_emu.xilinx_u55c_gen3x16_xdma_3_202210_1
rm -rf tmp_kernel_pack_merge_hw_xilinx_u50_gen3x16_xdma_5_202210_1
rm -rf tmp_kernel_pack_merge_hw_emu_xilinx_u50_gen3x16_xdma_5_202210_1
rm -rf packaged_kernel_merge_hw_xilinx_u50_gen3x16_xdma_5_202210_1
rm -rf packaged_kernel_merge_hw_emu_xilinx_u50_gen3x16_xdma_5_202210_1
rm -rf package.hw
rm -rf package.hw_emu
rm -rf build_dir.hw.xilinx_u50_gen3x16_xdma_5_202210_1
rm -rf build_dir.hw_emu.xilinx_u50_gen3x16_xdma_5_202210_1
rm -rf _x
rm -rf _x.hw.xilinx_u50_gen3x16_xdma_5_202210_1
rm -rf _x.hw_emu.xilinx_u50_gen3x16_xdma_5_202210_1
#rm rtl_user_managed
#rm merge
if [ -d .run ]; then
  echo ".run exists!"
  rm -rf .run
else
  echo ".run do not exists!"
fi
rm -rf .ipcache
rm -rf .Xil
rm -rf tmp_kernel_pack_merge_hw_xilinx_u55c_gen3x16_xdma_3_202210_1
rm -rf packaged_kernel_merge_hw_xilinx_u55c_gen3x16_xdma_3_202210_1
rm -rf _x.hw.xilinx_u55c_gen3x16_xdma_3_202210_1
rm -rf build_dir.hw.xilinx_u55c_gen3x16_xdma_3_202210_1
rm -rf tmp_kernel_pack_merge_hw_xilinx_u250_gen3x16_xdma_4_1_202210_1
rm -rf build_dir.hw.xilinx_u250_gen3x16_xdma_4_1_202210_1
rm -rf _x.hw.xilinx_u250_gen3x16_xdma_4_1_202210_1
rm -rf packaged_kernel_merge_hw_xilinx_u250_gen3x16_xdma_4_1_202210_1
rm -rf tmp_kernel_pack_merge_hw_emu_xilinx_u250_gen3x16_xdma_4_1_202210_1
rm -rf packaged_kernel_merge_hw_emu_xilinx_u250_gen3x16_xdma_4_1_202210_1
rm -rf build_dir.hw_emu.xilinx_u250_gen3x16_xdma_4_1_202210_1
rm -rf _x.hw_emu.xilinx_u250_gen3x16_xdma_4_1_202210_1
rm -rf tmp_kernel_pack_merge_hw_xilinx_u280_gen3x16_xdma_1_202211_1
rm -rf packaged_kernel_merge_hw_xilinx_u280_gen3x16_xdma_1_202211_1
rm -rf build_dir.hw.xilinx_u280_gen3x16_xdma_1_202211_1
rm -rf _x.hw.xilinx_u280_gen3x16_xdma_1_202211_1
rm *.log
rm *.jou
rm *.txt
pkill -f Xilinx
pkill -f xsim
rm -rf /home/smarques/rtl_user_managed/src/.run
