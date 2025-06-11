import 'package:flutter/material.dart';
import 'Main_page.dart';

class Clear_page extends StatelessWidget {
  const Clear_page({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.white,
      body: Center(
        child: Padding(
          padding: const EdgeInsets.symmetric(horizontal: 40.0),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.start,
            crossAxisAlignment: CrossAxisAlignment.center,
            children: [
              const SizedBox(height: 145),
              // SafeRide 로고
              Text(
                'SafeRide',
                style: TextStyle(
                  fontSize: 73,
                  fontWeight: FontWeight.bold,
                  letterSpacing: 1.5,
                  color: Colors.black,
                  shadows: [
                    Shadow(
                      blurRadius: 8.0,
                      color: Colors.black26,
                      offset: Offset(0, 4),
                    ),
                  ],
                ),
              ),
              const SizedBox(height: 15),
              // 파란 체크 아이콘
              Icon(
                Icons.check_circle_outline,
                color: Colors.blue,
                size: 220,
              ),
              const SizedBox(height: 15),
              // 안내문구
              Container(
                width: double.infinity,
                padding: const EdgeInsets.symmetric(vertical: 16, horizontal: 12),
                decoration: BoxDecoration(
                  color: Colors.blue[50],
                  border: Border.all(color: Colors.blue, width: 2),
                  borderRadius: BorderRadius.circular(10),
                ),
                child: const Text(
                  '충격 감지가 해제되었습니다.\n즐거운 라이딩 되시길 바랍니다.\n감사합니다.',
                  style: TextStyle(
                    color: Colors.blue,
                    fontSize: 18,
                    fontWeight: FontWeight.bold,
                  ),
                  textAlign: TextAlign.center,
                ),
              ),
              const SizedBox(height: 57),
              // 홈화면 돌아가기 버튼
              SizedBox(
                width: double.infinity,
                height: 50,
                child: ElevatedButton(
                  onPressed: () {
                    Navigator.push(
                      context,
                      MaterialPageRoute(
                        builder: (context) => const Main_page(),
                      ),
                    );
                  },
                  style: ElevatedButton.styleFrom(
                    backgroundColor: Colors.blue,
                    shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(10),
                    ),
                  ),
                  child: const Text(
                    '홈화면 돌아가기',
                    style: TextStyle(
                      color: Colors.white,
                      fontSize: 18,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
