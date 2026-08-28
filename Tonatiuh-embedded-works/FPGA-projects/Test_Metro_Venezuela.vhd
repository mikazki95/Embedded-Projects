--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   19:00:25 03/05/2025
-- Design Name:   
-- Module Name:   /home/ise/Compartido/TestD15X9_V2/D15X9.vhd
-- Project Name:  TestD15X9
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: BufferD15
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY D15X9 IS
END D15X9;
 
ARCHITECTURE behavior OF D15X9 IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT BufferD15
    PORT(
         X_1 : IN  std_logic;
         X_0 : IN  std_logic;
         SEL_1 : IN  std_logic;
         RD : IN  std_logic;
         WR : IN  std_logic;
         IO_0 : IN  std_logic;
         IO_1 : IN  std_logic;
         IO_2 : IN  std_logic;
         ALE_UC : IN  std_logic;
         X_2 : IN  std_logic;
         X_3 : IN  std_logic;
         I_0 : IN  std_logic;
         A7 : IN  std_logic;
         SEL_2 : IN  std_logic;
         SEL_0 : IN  std_logic;
         CS_0 : IN  std_logic;
         OUT_1 : OUT  std_logic;
         E_0 : OUT  std_logic;
         OUT_0 : OUT  std_logic;
         DIR_BUF : OUT  std_logic;
         O_ID : OUT  std_logic;
         EN_BUF : OUT  std_logic;
         EN_INPDL : OUT  std_logic;
         LE_INPDL : OUT  std_logic;
         LE_OPDL : OUT  std_logic;
         RST_UC : OUT  std_logic;
         LE_OPDH : OUT  std_logic;
         INT_WR : OUT  std_logic;
         INT_RD : OUT  std_logic;
         STATE_2 : OUT  std_logic;
         EN_INPDH : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal X_1 : std_logic := '0';
   signal X_0 : std_logic := '0';
   signal SEL_1 : std_logic := '0';
   signal RD : std_logic := '0';
   signal WR : std_logic := '0';
   signal IO_0 : std_logic := '0';
   signal IO_1 : std_logic := '0';
   signal IO_2 : std_logic := '0';
   signal ALE_UC : std_logic := '0';
   signal X_2 : std_logic := '0';
   signal X_3 : std_logic := '0';
   signal I_0 : std_logic := '0';
   signal A7 : std_logic := '0';
   signal SEL_2 : std_logic := '0';
   signal SEL_0 : std_logic := '0';
   signal CS_0 : std_logic := '0';

 	--Outputs
   signal OUT_1 : std_logic;
   signal E_0 : std_logic;
   signal OUT_0 : std_logic;
   signal DIR_BUF : std_logic;
   signal O_ID : std_logic;
   signal EN_BUF : std_logic;
   signal EN_INPDL : std_logic;
   signal LE_INPDL : std_logic;
   signal LE_OPDL : std_logic;
   signal RST_UC : std_logic;
   signal LE_OPDH : std_logic;
   signal INT_WR : std_logic;
   signal INT_RD : std_logic;
   signal STATE_2 : std_logic;
   signal EN_INPDH : std_logic;
   -- No clocks detected in port list. Replace <clock> below with 
   -- appropriate port name 
 
 --  constant <clock>_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: BufferD15 PORT MAP (
          X_1 => X_1,
          X_0 => X_0,
          SEL_1 => SEL_1,
          RD => RD,
          WR => WR,
          IO_0 => IO_0,
          IO_1 => IO_1,
          IO_2 => IO_2,
          ALE_UC => ALE_UC,
          X_2 => X_2,
          X_3 => X_3,
          I_0 => I_0,
          A7 => A7,
          SEL_2 => SEL_2,
          SEL_0 => SEL_0,
          CS_0 => CS_0,
          OUT_1 => OUT_1,
          E_0 => E_0,
          OUT_0 => OUT_0,
          DIR_BUF => DIR_BUF,
          O_ID => O_ID,
          EN_BUF => EN_BUF,
          EN_INPDL => EN_INPDL,
          LE_INPDL => LE_INPDL,
          LE_OPDL => LE_OPDL,
          RST_UC => RST_UC,
          LE_OPDH => LE_OPDH,
          INT_WR => INT_WR,
          INT_RD => INT_RD,
          STATE_2 => STATE_2,
          EN_INPDH => EN_INPDH
        );

   -- Clock process definitions
   CLK_process :process
   begin
	CLK <= '0';
		wait for CLK_period/2;
		CLK <= '1';
		wait for CLK_period/2;
   end process;
 
process
begin
	wait for 100 ns;
	CS_0 <= '1';
	X_1 <= '0';
   X_0 <= '0';
	SEL_2 <= '0';
   SEL_1 <= '0';
	SEL_0 <= '0';
   RD <= '0';
   WR <= '0';
   IO_0 <= '0';
   IO_1 <= '0';
   IO_2 <= '0';
	A7 <= '0';
----------------------------------------
	wait for 100 ns; -- Total: 300 ns
	CS_0 <= '0';
	X_1 <= '1';
   X_0 <= '1';
	SEL_2 <= '0';
   SEL_1 <= '1';
	SEL_0 <= '0';
   RD <= '1';
   WR <= '1';
   IO_0 <= '1';
   IO_1 <= '0';
   IO_2 <= '0';
	A7 <= '0';
	
	----------------------------------------
wait for 300 ns; -- Total: 300 + T ns
	CS_0 <= '0';
	X_1 <= '1';
   X_0 <= '1';
	SEL_2 <= '1';
   SEL_1 <= '1';
	SEL_0 <= '0';
   RD <= '1';
   WR <= '1';
   IO_0 <= '1';
   IO_1 <= '0';
   IO_2 <= '0';
	A7 <= '0';
----------------------------------------
wait for 300 ns; -- Total: 300 + T ns
	CS_0 <= '0';
	X_1 <= '1';
   X_0 <= '1';
	SEL_2 <= '0';
   SEL_1 <= '0';
	SEL_0 <= '1';
   RD <= '1';
   WR <= '1';
   IO_0 <= '1';
   IO_1 <= '0';
   IO_2 <= '0';
	A7 <= '0';
----------------------------------------
wait for 100 ns;
end process;
   -- Stimulus process
 --  stim_proc: process
  -- begin		
      -- hold reset state for 100 ns.
    --  wait for 100 ns;	

    --  wait for <clock>_period*10;

      -- insert stimulus here 

     -- wait;
   --end process;

END;
