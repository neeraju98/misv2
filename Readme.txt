4-8-2021	1.0.0.1		Add UpperThreshold in Start, AutoCapture Api.
						Set UpperThreshold=85, LowerThreshold=55 from GUI
5-8-2021	1.0.0.2		Add NoOfframeCashing in Start and AutoCapture.
						Set NoOfframeCashing=6
18-8-2021	1.0.0.3		Add Dual Thread Logic for image acquisition in comm library.
						Solve K7 image problem from iEngine.
24-8-2021	1.0.0.4		Solve LED exception problem.
25-8-2021	1.0.0.4		change error description.
27-8-2021	1.0.0.5		Capture image with high fps - comm library
03-9-2021	1.0.0.6		Remove Lower, Upper threshold form StartCapture.
						Add AutoCapture for RD with Lower, Upper, FrameCash Parameter.
						Remove Lower, Upper, FrameCash from regular AutoCapture.
06-9-2021	1.0.0.7		add try-catch inside while loop -> DeviceDetectionThread()
07-9-2021	1.0.0.7_2	change MR014_MIS100V2_Windows_Auth_IPL.dll, without build
13-9-2021	1.0.0.7_3	change MIDIRIS_Auth_GetImage() with all format in Match Template, without build
21-9-2021	1.0.0.8		Solve memory leakage problem in ipl library, and  tester (Draw Eclips)
22-9-2021	1.0.0.8		change icon in sample
24-9-2021	1.0.0.9		release memory from match iris. Clear picture box when uninit and remove device.
24-9-2021	1.0.0.10	Change Buffer and Queue Size from CommLib.
27-9-2021	1.0.0.11	Optimize Core Code in Callback (remove NULL from callback).
27-10-2021	1.0.0.12	Validate MFG Data() -> For DPI in InitDevice().
09-12-2021	1.0.0.13_Beta	Commlib: change capturing logic. use dual thread and linklist.
08-04-2021	1.0.0.14	Commlib: optimized image parsing linklist logic.
09-06-2021	1.0.0.14	Set language as Nutural in version detail.
15-09-2022	1.0.0.15	remove third party dll file and bind with static lib file also update commlib(1.0.0.11) replace ipl(7-9-2022-2) static MT lib
20-02-2023	1.0.0.16	Fix error in MatchIris function (ire3_settings structure) ire3_settings allocated from IPL.
						
						