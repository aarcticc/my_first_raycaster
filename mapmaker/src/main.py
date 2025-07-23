import sys
from gui.editor import Editor
from PyQt5.QtWidgets import QApplication

def main():
    app = QApplication(sys.argv)
    editor = Editor()
    editor.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()